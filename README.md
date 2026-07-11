# PCF8575-HD44780

<img width="734" height="299" alt="8BIT 12C HD44780" src="https://github.com/user-attachments/assets/4a51e56c-c4e3-4eed-8347-276308d4dafb" />

# Hardware PCB using a PCF8575 I2C 8bit custom interface for LCD HD44780

# Software driver provided for STM32 "BlackPill" board development.

There is to my knowledge only one 8 Bit I2C interface specifically sold to interface directly with the pins of an HD44780LCD display

<img width="220" height="220" alt="image" src="https://github.com/user-attachments/assets/94d41794-8135-408f-bd76-779d75eb73db" />

  image curtosy of "diymore Alice1101983 Store"
  
It uses the MCP23017 chip to provide all the control bits and data bits required to run an hd44780 LCD in 8 bit mode. I have written code for it to run on a STM32 "BlackPill" development board. It works, its fast, and it easily reads the memory from the HD44780 bus. 
This function is possibly in 4 bit mode as 

<img width="490" height="490" alt="image" src="https://github.com/user-attachments/assets/a8dc73bd-d262-4e09-81e6-08df36fd81bb" />

image curtosy of "Cricklewood Electronics"
