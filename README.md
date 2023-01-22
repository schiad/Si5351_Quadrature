# Si5351_Quadrature

Clock generator with quadrature phase signal and OLED display.  
It can be used in many RF applications.

## Material list

1 [arduino](https://www.adafruit.com/product/2488 "Arduino")  
1 [Si5351A module](https://www.adafruit.com/product/2045 "Si5351 module")  
1 [OLED screen display with 128x32](https://www.adafruit.com/product/661 "OLED Display")  
1 [Incremental encoder](http://www.elma.com/en/products/rotary-switches/rotary-switching-products/product-pages/type-e33-detail/ "Incremental encoder")  

## Schematics 

![Schematics](/images/schematics.png)

## Little guide of usage

This code makes 3 outputs CLK0, CLK1 and CLK2, it makes the same frequency in CLK0 and CLK1  
with a quadrature (90° phase shift) from about 3.2 MHz and 56.2 MHz, CLK2 is an independent output.


The LED screen indications : 

**Clk|Frequency    |Phy**  
**0    3 500 000 Hz0-90**	<- Frequency and Phy For CLK0 and CLK1  
**2    6 000 000 Hz   0**	<- Frequency for CLK2  
**.  720 000 000 Hz   .**	<- Frequency for last PLL called  

Use the encoder to change the frequency, rotate counterclockwise (CW) to up the frequency,  
rotate anticlockwise (CCW) to down the frequency.  
To change the multiple of step make short one click on encoder.  
To jump from CLK0-1 to CLK2 frequency setup makes a long push on encoder.  

## Future features and bug to resolve

- [ ] Resolve the bug of 0 before frequency for the unit indicator  
- [ ] See code for Serial control
- [ ] Add preprocessor directives to enable/disable optional display.

## Libraries used or copied

Copied and modified	: [etherkit/Si5351](https://github.com/etherkit/Si5351Arduino "Library for Si5351")  

You must install these libraries :  
[adafruit/Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library "Adafruit-GFX-Library")  
[adafruit/Adafruit\_SSD1306](https://github.com/adafruit/Adafruit_SSD1306 "Adafruit_SSD1306")  

## Thanks to

Thanks to **F4IEA** for his help and the idea for this project.

Thanks to libraries writers.  

Thanks to use, share and report any bugs.  
