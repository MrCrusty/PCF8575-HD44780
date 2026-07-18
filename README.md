# PCF8575-HD44780

<img width="734" height="299" alt="8BIT 12C HD44780" src="https://github.com/user-attachments/assets/4a51e56c-c4e3-4eed-8347-276308d4dafb" />

# The Kicad 10.x 3D rendered image above shows a PCB designed to use a PCF8575 I2C two port 8bit custom interface for HD44780 LCD's

# A C99 Software driver is provided for STM32 "BlackPill" development board, which should be easily modifed for other MCU's
 
### Hitachi brought the HD44780 to market 40 years ago, at that time I was just turning 40 and in that year my employer complained at the exhorbitant cost of using this display in a digital multimeter that could have quite have easily been an off the peg analogue AVO.

### Today these LCd's are sub £5.00 for a 4 line by 20 character display. However interfacing this LCD in it's native 8bit data bus mode uses to many I/O pins on an embeded MCU design. The solution has been to use I2C interface modules notably the PCF8574 which has become popular as they only require the MCU to use 2 I/O pins (SDA and SCK) to effect communication between the LCD and the MCU. 
<img width="490" height="490" alt="image" src="https://github.com/user-attachments/assets/a8dc73bd-d262-4e09-81e6-08df36fd81bb" />
image curtosy of "Cricklewood Electronics"

There is to my knowledge only one 8 Bit I2C interface specifically sold to interface directly with the pins of an HD44780LCD display

#### There is a lot of excelent code that suports these I2C port extender modules. There is a trade off with using only an 8 bit extender port which is that the display has to be operated in 4 bit bus mode so that the other 4 bits can be used for the displays control lines. Whilst there is a lot of code for writing to a display in 4 bit mode, there is a very limited amount of code to be found for reading the display memory in 4 bit mode. 

## So is it possible to write to the Hd44780 with I2C in 8 bit mode yes it is not very difficult and a commercial module for doing this is as shown below
<img width="220" height="220" alt="image" src="https://github.com/user-attachments/assets/94d41794-8135-408f-bd76-779d75eb73db" />
image curtosy of "diymore Alice1101983 Store"
  
It uses the MCP23017 chip to provide all the control bits and data bits required to run an hd44780 LCD in 8 bit mode. I have written code for it to run on a STM32 "BlackPill" development board. It works, its fast, and it easily reads the memory from the HD44780 bus. 

## So why design and build my own I2C dual 8 bit port device using the PCF8575 the answer is that the MCP23017 chip needs at least one more I/O pin to be supplied by the MCU in the guise of a reset line, this is essential in my view as the adressing mode of the device can be two diferent options. The other problem for me is that it has far more registers than I need to run the LCD.



