#include <SPI.h>
#include "./s5351.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 0);
Si5351 si5351;
int stata[2] = {0, 0};
int statb[2] = {0, 0};
int stat = 0;
char ligne[21];
int power = 6;
uint64_t frequ[2] = {5000000, 6000000};
bool change = 1;
bool bp[2] = {0, 0};
int32_t  chanal = 0;
static int clk = 0;


/*#define LOGO_HEIGHT   16
  #define LOGO_WIDTH    16

  static const unsigned char PROGMEM logo_bmp[] =
  { B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
  };*/

void setup() {

  bool i2c_found;

  Serial.begin(115200);
  Wire.setClock(400000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.clearDisplay();
  delay(10);
  display.display();
  display.setTextSize(1);
  display.setTextColor(BLACK);


  i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 73765);

  if (!i2c_found)
  {
    Serial.println("SI5351 not found on I2C bus!");
    display.setCursor(1, 0);
    display.println("Si5351 not found !");
    display.println("please check");
    display.println("connectors.");
    display.println("");
    display.display();
    while (1);
  }

  si5351.set_ms_source(SI5351_CLK0, SI5351_PLLA);
  si5351.set_ms_source(SI5351_CLK1, SI5351_PLLA);
  si5351.set_ms_source(SI5351_CLK2, SI5351_PLLB);

  display.setCursor(1, 0);
  display.fillRect(0, 0, 128, 32, WHITE);
  display.println("Square wave generator");
  display.println("Welcome F4IFB");
  display.println("V0.5");
  display.println("Beta tests");
  display.display();
  delay(2000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(1, 0);
  display.println("Clk|Frequency    |Phy");
  display.println("01             Hz0-90");
  display.println("2              Hz   0");
  display.display();

  frequency(frequ[0]);
  clk = 1;
  frequency(frequ[1]);
  clk = 0;

  frequency_disp(frequ[0], '0', 1);
  frequency_disp(frequ[1], '0' + 1, 0);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), interrupt,  CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), interrupt,  CHANGE);
  interrupt();
}

uint64_t  p10(uint64_t n)
{
  int i = 0;
  uint64_t ret = 1;

  while (i < n)
  {
    ret *= 10;
    i++;
  }
  return ret;
}

void interrupt()
{
  stata[0] = digitalRead(2);
  statb[0] = digitalRead(3);
  if (stata[0] != stata[1] || statb[0] != statb[1])
  {
    if (stata[0] == statb[0])
    {
      if (stata[0] == stata[1])
      {
        stat++;
      } else {
        stat--;
      }
    }
    stata[1] = stata[0];
    statb[1] = statb[0];
  }
  frequ[clk] = frequ[clk] + (stat * p10(power));
  frequ[clk] /= p10(power);
  frequ[clk] *= p10(power);
  if (frequ[clk] > 200000000)
  {
    frequ[clk] = 200000000;
  }
  if (frequ[clk] < 2500)
  {
    frequ[clk] = 2500;
  }
  Serial.println((long)frequ[clk]);
  Serial.println(power);
  stat = 0;
  change = 1;
}

uint8_t bargraph(uint8_t num, ...)
{
  static uint8_t x_start, y_start, y, mode, x, old_x;
  va_list varlist;

  va_start(varlist, num);

  x = va_arg(varlist, unsigned int);

  if (num == 2)
  {
    y_start = va_arg(varlist, unsigned int);
    x_start = va_arg(varlist, unsigned int);
    y       = va_arg(varlist, unsigned int);
    mode    = va_arg(varlist, unsigned int);
    old_x = 0;
  }

  va_end(varlist);

  if (x > old_x + x_start)
    display.fillRect(x_start + old_x, y_start, x - old_x, y, mode);
  else
    display.fillRect(x_start + x, y_start, old_x - x, y, mode);
  display.display();

  old_x = x;

  return 0;
}

int64_t  ft_atoi(char *str)
{
  uint8_t i, negative;
  int64_t nbr = 0;
  i = 0;

  while ((str[i] == '\n') || (str[i] == '\t') || (str[i] == '\v') ||
         (str[i] == ' ') || (str[i] == '\f') || (str[i] == '\r'))
    i++;
  if (str[i] == '-')
    negative = 1;
  if (str[i] == '+' || str[i] == '-')
    i++;
  while (str[i] && (str[i] >= '0') && (str[i] <= '9'))
  {
    nbr *= 10;
    nbr += (uint64_t)str[i] - '0';
    i++;
  }
  return nbr;
}

void char_disp(uint8_t line, uint8_t col, char str)
{
  display.setCursor(line, col);
  display.print(str);
}

void frequency_disp(int64_t freqq, char chanall, int mark)
{
  uint8_t i = 0;
  uint8_t j = 0;
  uint8_t line;
  char n;

  switch  (chanall)
  {
    case '0':
      line = 8;
      break;
    case '1':
      line = 16;
      break;
    case '2':
      line = 24;
      break;
  }
  
  display.fillRect(8, line, 80, 8, BLACK);
  while (i < 10 && (13 - i))
  {
    n = (freqq % 10) + '0';
    char_disp((13 - (i + j)) * 6 , line , n);
    if ((i == power) && (chanall = '0') && mark) {
      display.fillRect(((13 - (i + j)) * 6) - 1, line, 7, 7, INVERSE);
    }
    freqq = freqq / 10;
    if (freqq && ((i % 3) == 2))
    {
      j++;
    }
    i++;
  }

  display.display();
}

unsigned long pulsein(int pin, int len)
{
  unsigned long start = millis();
  if (digitalRead(4) == LOW)
  {
    while ( (digitalRead(4) == LOW) && ((millis() - start) < len) );
    return (millis() - start);
  }
  else
  {
    ;
  }
  return 0;
}

void loop() {
  //static char chanal = 0;
  //static uint64_t clk_d = 0;
  static int timepressed = 0;

  /*if (Serial.available() && 0)
    {
    chanal = Serial.read();
    switch  (chanal)
    {
      case '0':
        clk_d = SI5351_CLK0;
        break;
      case '1':
        clk_d = SI5351_CLK1;
        break;
    }
    delay(100);
    uint8_t i = 0;
    char c_freq[10];

    while (Serial.available())
    {
      c_freq[i] = Serial.read();
      i++;
    }
    c_freq[i] = 0;
    freq = ft_atoi(c_freq) ;
    //Serial.println(freq);
    if (power < 5)
    {
      si5351.set_freq1(freq * 100, clk_d);
    } else
    {
      si5351.set_freq(freq * 100, clk_d);
    }
    //si5351.update_status();
    frequency_disp(freq, clk_d);
    display.display();
    Serial.print(chanal);
    Serial.print(" : ");
    Serial.println((long)freq);
    }*/


  bp[0] = digitalRead(4);
  if (!bp[0] && bp[1])
    timepressed = pulsein(digitalRead(4), 1000);
  if (timepressed)
    Serial.println(timepressed);


  if (timepressed && timepressed < 900)
  {
    Serial.println("ls1");
    power--;
    if (power < 0)
      power = 8;
    frequency_disp(frequ[clk], '0' + clk, 1);
  } else if (timepressed >= 900)
  {
    if (!clk)
    {
      clk = 1;
    }
    else
    {
      clk = 0;
    }

    Serial.print("CLK ");
    Serial.println(clk);
  }

  bp[1] = bp[0];
  timepressed = 0;

  if (change > 0)
  {
    change = 0;
    Serial.println(" change");

    if (frequency(frequ[clk] * 100))
    {
      frequency_disp(frequ[clk], '0' + clk, 1);
      ;
    }
    else
    {
      frequ[clk] = 3500000;
      frequency(frequ[clk] * 100);
      ;
    }
    frequency_disp(frequ[clk], '0' + clk, 1);
    display.display();
  }
}


int  frequency(uint64_t freq)
{
  static uint64_t pll_freq = 0;
  const uint64_t pll_min =  40000000000;
  const uint64_t pll_min2 = 60000000000;
  const uint64_t pll_max =  90000000000;
  static uint16_t i;
  static int R;
  static int ok;
  static uint16_t mul;
  static uint16_t p2;
  ok = 0;



  R = 8;
  if (clk)
  {
    while (!ok && R)
    {
      i = 8;
      Serial.println(".");
      p2 = pow(2, R);
      while (!ok && i <= 900)
      {
        pll_freq = freq * i * p2;
        if (pll_freq >= pll_min2 && pll_freq <= pll_max)
        {
          ok = 1;
          Serial.println((unsigned long)pll_freq);
          frequency_disp(pll_freq / 100, '2', 0);
          Serial.println("OK");
          si5351.set_freq_manual(freq, pll_freq, SI5351_CLK2);
          si5351.pll_reset(SI5351_PLLB);
          si5351.update_status();
        }
        i++;
      }
      R--;
    }
    si5351.set_freq_manual(freq, pll_freq, SI5351_CLK2);
    ok = 1;
  }
  else
  {
    while (!ok && R)
    {
      i = 8;
      Serial.println(".");
      p2 = pow(2, R);
      while (!ok && (p2 * i) <= 127)
      {
        pll_freq = freq * i * p2;

        if (pll_freq >= pll_min && pll_freq <= pll_max)
        {
          ok = 1;
          Serial.println((unsigned long)pll_freq);
          frequency_disp(pll_freq / 100, '2', 0);
          Serial.println("OK");
          si5351.set_freq_manual(freq, pll_freq, SI5351_CLK0);
          si5351.set_freq_manual(freq, pll_freq, SI5351_CLK1);
          si5351.set_phase(SI5351_CLK0, 0);
          si5351.set_phase(SI5351_CLK1, i * p2);
          si5351.pll_reset(SI5351_PLLA);
          si5351.update_status();
        }
        i++;
      }
      R--;
    }
  }
  return ok;
}
