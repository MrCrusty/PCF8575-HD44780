/*
 * hd44780_12c_8bit.h
 *
 *  Created on: 8 Jun 2026
 *      Author: mrcru
 */

#ifndef INC_HD44780_12C_8BIT_H_
#define INC_HD44780_12C_8BIT_H_

#include "main.h"
#include "i2c.h"
#include "stdbool.h"
 #include "string.h"
#include <stdio.h>
#include "stdlib.h"
#define CONTROL_BYTE              	0b00000000// basestate xxxxRS,R/W,EN,BL
#define CLEAR_DISPLAY              	0b00000001//
#define RETURN_HOME              	0b00000010//
#define ENTRY_MODE_SET          	0b00000100// DB1, DB0 modifiable
#define DISPLAY_ON		   		    0b00001100// DB2, DB1, DB0 modifiable
#define DISPLAY_OFF					0b00001000// DB2, DB1, DB0 modifiable
#define CURSOR_DISPLAY_SHIFT  0b00010000// DB3, DB2 modifiable
#define FUNCTION_SET				0b00100000// DB4, DB3, DB2 modifiable
#define SET_CGRAM_ADDRESS		0b01000000// DB5, DB4, DB3, DB2, DB1, DB0 modifiable
#define SET_DDRAM_ADDRESS 	0b10000000// DB6, DB5, DB4, DB3, DB2, DB1, DB0 modifiable
//defines the active  bits used by port P0 which is first addressed
#define RS 					7
#define RW 					6
#define EN 					5
#define BL 					4
#define READ 				1
#define WRITE				0
#define INSTRUCTION 	0
#define DATA				1

#define BACK_LIGHT_ON  1
#define BACK_LIGHT_OFF 0

		#define HD_BL_ON      1
		#define HD_BL_OFF      0
		#define HD44780_clr_eol     1
		#define HD_DISPLAY_BIT 2
		#define HD_CURSOR_BIT 1
		#define HD_BLINK_BIT 0
		#define HD_DISPLAY_ON 1
		#define HD_CURSOR_ON 1
		#define HD_BLINK_ON 1
		#define ON true
		#define OFF false
		#define RS_INSTRUCTION 	0
		#define RS_DATA					1
		#define HD_WRITE				0
		#define HD_READ    				1
		#define HD_EN						1
		#define HD_DL    4
		#define HD_N     3
		#define HD_F		2
		#define HD_D  	2
		#define HD_C  		1
		#define HD_B  		0
		#define HD_ID 		1
		#define HD_S   	0
		#define HD_DISPLAY_WIDTH 				19
		#define HD_LINE0 								0x00
		#define HD_LINE1 								0x40
		#define HD_LINE2 								0x14
		#define HD_LINE3 								0x54
		#define HD_SLAVE 0x21
#define NO_LINE_CLR 0x00
#define LINE_CLEAR 0x01
#define POSITION_ONLY 0x00
#define READ_TO_EOL  HD_DISPLAY_WIDTH
#define PCF_8575 1
#define PCF_8574 2
#define MCP_23017 3
#define STRING 0
#define NUMBER 1

	typedef struct {
	uint8_t array[21];
	} LineStruct;

void hd44780_cursor_state(uint8_t device_add,bool display ,bool cursor, bool blink);
void hd44780_write_instruction_8575(uint8_t device_add,uint8_t HD_data_byte);
void hd44780_init_8bit(uint8_t device_add, _Bool BackLight);
void hd44780_set_line(uint8_t device_add, uint8_t line_no, uint8_t cursor_position, bool clear_line);
void hd44780_write_character_at(uint8_t device_add, char character,uint8_t line, uint8_t cursor,bool clear_line);
void hd44780_write_character(uint8_t device_add,char character);
void hd44780_write_message(uint8_t device_add,char message[]);
void hd44780_write_message_at(uint8_t device_add,char message[], uint8_t line, uint8_t cursor, bool clear_line);
void hd44780_clr_disp(uint8_t device_add);
void hd44780_custom_char(uint8_t device_add,uint8_t add_of_char, char patten[], uint8_t patten_size);
uint8_t hd44780_read_single_byte(uint8_t device_add,uint8_t line_no, uint8_t cursor_position);
LineStruct hd44780_read_bulk_bytes(uint8_t device_add,uint8_t line_no, uint8_t cursor_position, _Bool read_to_end);
LineStruct hd44780_read_line_from(uint8_t device_add, uint8_t line_no, uint8_t start_cursor, uint8_t finish_cursor);
void hd44780_write_line_from(uint8_t device_add, uint8_t line_no, uint8_t cursor_position, LineStruct chars_to_write,uint8_t char_count );
//void hd44780_write_bulk_bytes(uint8_t device_add, char HD_data[],int str_num);
void hd44780_format16_dec(uint16_t Value, char * Ans);
void hd44780_format8_dec(uint8_t Value, char * Ans);
#endif /* INC_HD44780_12C_8BIT_H_ */
