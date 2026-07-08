/*
 * hd44780_i2c_8bit.c
 *
 *  Created on: 8 Jun 2026
 *      Author: mrcru
 */
#include "hd44780_12c_8bit.h"
int bit_modify(int x, unsigned char position, bool new_state); //local forward declaration

const uint8_t  Line0 = 0x00;
const uint8_t  Line1 = 0x40;
const uint8_t  Line2 = 0x14;
const uint8_t  Line3 = 0x54;
static uint8_t text_count;
static uint8_t text_line;
static char text_line_0[20];
static char text_line_1[20];
static char text_line_2[20];
static char text_line_3[20];
const uint8_t DisplayWidth = 20;

typedef struct
	{
		uint8_t chip_type;
		uint8_t device_add;
		uint8_t lines;
		uint8_t characters;
	} I2C_Process_handler;

I2C_Process_handler Interface8575 =
	{
		.chip_type = PCF_8575,
		.device_add = 0x21,
		.lines = 4,
		.characters = 20
	};




/**
 	 *Max number of characters on a 20*4 line screen = 80 elements
 	 *To send in bulk with only the use of device address once
 	 *requires 2 passes .
 	 *Pass 1 is
 	 *Control byte followed by data byte with no enable set in the control byte
 	 *this allows setup time
 	 *Pass 2 is
 	 *Control byte followed by data byte with with enable set in the control byte
 	 *this allows write to happen data byte is the same
 	 *this is so the next control byte and data sent go to the correct 2 registers
*/
char I2C_send_buffer[320];//buffer for HAL I2C functions
_Bool back_light_state;

void hd44780_backlight_on_off(_Bool state)
{
	back_light_state = state;
}


void hd44780_write_instruction(uint8_t device_add,uint8_t HD_data_byte)
{
	uint8_t HD_control_byte =0b00100000;//bit 7 set for instruction, bit 6 set for write, bit 5 set not to enable
	HD_control_byte = bit_modify(HD_control_byte, BL, back_light_state);//setbit 4 for the baclight state
	I2C_send_buffer[0]= HD_control_byte;//put in buffer
	I2C_send_buffer[1]= HD_data_byte;//put in buffer
	HAL_I2C_Master_Transmit( &hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//activate the write
	HAL_Delay(1);
	HD_control_byte = bit_modify(HD_control_byte, EN, 0);//disable the write
	I2C_send_buffer[0]= HD_control_byte;//send to buffer
	HAL_I2C_Master_Transmit( &hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//deactivate the write
	HAL_Delay(1);
}

/**
 *  Writes a single data byte  to the HD44780  in 8 bit mode.
  * @variable (control_byte local variable, holding the HD44780 bits RS,RW,EN and Backlight
  * @variable (bl_state) global variable, used to set baklight bit on each pass of function.
  * @param   (slave_add) hardware address of slave
  * @param  (HD_data[] )used to sequentially load the 8bit data  to the HD44780 data bus.
  * @retval no return value as yet
  */
void hd44780_write_single_byte(uint8_t device_add, uint8_t HD_data_byte)
{
	uint8_t HD_control_byte =0b10100000;//bit 7 set for data bit 6 set for write, bit 5 set not to enable
	HD_control_byte = bit_modify(HD_control_byte, BL, back_light_state);//setbit 4 for the baclight state
	I2C_send_buffer[0]= HD_control_byte;//put in buffer
	I2C_send_buffer[1]= HD_data_byte;//put in buffer
	HAL_I2C_Master_Transmit(&hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//activate the write
	HD_control_byte = bit_modify(HD_control_byte, EN, 0);//disable the write
	I2C_send_buffer[0]= HD_control_byte;//send to buffer
	HAL_I2C_Master_Transmit( &hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//deactivate the write
}

/**
 *	@Brief Writes a block of data byte  to the HD44780  in 8 bit mode.
 *  @Brief This is achieved by sending control bytes separated by data bytes.
 *  @Brief 8575 I2C will roll through its two port registers until a new adress is sent.
  * @Brief PLEASE NOTE CGRAM or DDRAM read/write need  SET_CGRAM_ADDRESS / SET_DDRAM_ADDRESS
  * @Brief prior to calling any memory read / write function
  * @variable (control_byte local variable, holding the HD44780 bits RS,RW,EN and Backlight
  * @variable (bl_state) global variable, used to set baklight bit on each pass of function.
  * @param   (slave_add) hardware address of slave
  * @param  (HD_data[] )used to sequentially load the 8bit data  to the HD44780 data bus.
  * @param int data_count used to extract each element of the message until 0 or null termination reached
  * @param buffer_pos keeps count of the bytes used in the array sent to HAL_I2C master transmit function
  * @retval no return value as yet
  */
void hd44780_write_bulk_bytes(uint8_t device_add, char HD_data[],int str_num)
{
	uint8_t control_byte_no_enable=0b10000000;//bit 7 set for data bit 6 set for write, bit 5 not set for enable
	control_byte_no_enable = bit_modify(control_byte_no_enable, BL, back_light_state);//set bit 4 for the baclight state
	uint8_t control_byte_enable=0b10100000;//bit 7 set for data bit 6 set for write, bit 5 set to enable
	control_byte_enable = bit_modify(control_byte_enable, BL, back_light_state);//setbit 4 for the baclight state
	int data_count = 0;
	uint16_t buffer_pos = 0;
	if(str_num == STRING )
	{
		while(HD_data[data_count] != 0)//test for terminator character of string
		{
			I2C_send_buffer[buffer_pos] = control_byte_enable;//set not enabled
			buffer_pos ++;// inc the buffer count to next array element
			I2C_send_buffer[buffer_pos]= HD_data[data_count];//add the data register value
			buffer_pos ++;// inc the buffer count to next array element
			//first pass done to set up data and control without hd44780 enable
			I2C_send_buffer[buffer_pos] = control_byte_no_enable;//set so enabled
			buffer_pos++;//set next buffer location
			I2C_send_buffer[buffer_pos]= HD_data[data_count];//add the data register value to remain same while write enabled
			buffer_pos++;//set next buffer location
			data_count++; // set to read the next data byte
		}
		HAL_I2C_Master_Transmit( &hi2c1, (device_add <<1), I2C_send_buffer, buffer_pos, 200);//deactivate the write

	}
	else //its here so its a number array may contain 0 so needs a size of the number array
	{
		//set how many elements to store as this data contains valid zeros
		while(data_count < str_num)//test for length of array reached
		{
			I2C_send_buffer[buffer_pos] = control_byte_enable;//set not enabled
			buffer_pos ++;// inc the buffer count to next array element
			I2C_send_buffer[buffer_pos]= HD_data[data_count];//add the data register value
			buffer_pos ++;// inc the buffer count to next array element
			//first pass done to set up data and control without hd44780 enable
			I2C_send_buffer[buffer_pos] = control_byte_no_enable;//set so enabled
			buffer_pos++;//set next buffer location
			I2C_send_buffer[buffer_pos]= HD_data[data_count];//add the data register value to remain same while write enabled
			buffer_pos++;//set next buffer location
			data_count++; // set to read the next data byte
		}
		HAL_I2C_Master_Transmit( &hi2c1, (device_add<<1), I2C_send_buffer,buffer_pos,200);//deactivate the write
	}

}



static uint8_t I2C_read_buf[20];
uint8_t hd44780_read_single_byte(uint8_t device_add,uint8_t line_no, uint8_t cursor_position)
{
	uint8_t pos_byte;

	uint8_t control_byte_no_enable=0b11000000;//bit 7 set for data bit 6 set for read, bit 5 not set for enable
	uint8_t control_byte_enable=0b11100000;//bit 7 set for data bit 6 set for read, bit 5 set to enable

	//set the backlight state of above local variables
	control_byte_no_enable = bit_modify(control_byte_no_enable, BL, back_light_state);//set bit 4 for the baclight state
	control_byte_enable = bit_modify(control_byte_enable, BL, back_light_state);//set bit 4 for the baclight state
	hd44780_set_line(device_add, line_no, cursor_position, false);//DDRAM memory address set to read from
	I2C_send_buffer[0]= control_byte_enable;//put in control port as a read function as previously found display needs enable first
	I2C_send_buffer[1]= 0xFF ;//put data port for read by sending 1's the data port now useable as read port
	HAL_I2C_Master_Transmit(&hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//activate the read
	HAL_I2C_Mem_Read(&hi2c1, (device_add <<1), 1, 2, I2C_read_buf, 2, 200);
	pos_byte = I2C_read_buf[1];// store the 8 bit data port byte read
	HAL_Delay(10);
	I2C_send_buffer[0]= control_byte_no_enable;// toggle control port to diabale display enable
	HAL_I2C_Master_Transmit(&hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//de_activate the read
	hd44780_set_line((device_add<<1), 3, 17, false);//DDRAM memory address set to read from

	hd44780_write_single_byte((device_add <<1), pos_byte);
	return(pos_byte);
}
/*
#include <stdio.h>



ArrayStruct createStructArray() {
ArrayStruct arrStruct;
for (int i = 0; i < 5; i++) {
arrStruct.array[i] = i * 2;
}
return arrStruct;
}

int main() {
ArrayStruct myArrayStruct = createStructArray();
for (int i = 0; i < 5; i++) {
printf("%d ", myArrayStruct.array[i]);
}
return 0;
}
*/


LineStruct hd44780_read_bulk_bytes(uint8_t device_add,uint8_t line_no, uint8_t cursor_position, _Bool read_to_end)
{
   LineStruct line_array;
	//local varaibles
	static uint8_t I2C_read_buf[20];
	uint8_t control_byte_no_enable=0b11000000;//bit 7 set for data bit 6 set for read, bit 5 not set for enable
	uint8_t control_byte_enable=0b11100000;//bit 7 set for data bit 6 set for read, bit 5 set to enable

	//set the backlight state of above local variables
	control_byte_no_enable = bit_modify(control_byte_no_enable, BL, back_light_state);//set bit 4 for the baclight state
	control_byte_enable = bit_modify(control_byte_enable, BL, back_light_state);//set bit 4 for the baclight state

	//The read process for 8575 chip
	uint8_t read_count; // holds the number of 8 bit bytes to be read from the specfied cursor position for display width
	if( read_to_end == true) read_count = HD_DISPLAY_WIDTH - cursor_position; //calculate number of bytes to read
	else read_count = 1; //just read the character at this point
	uint8_t bytes_read =0;//this counts the bytes as they are read
	hd44780_set_line(device_add, line_no, cursor_position, false);//DDRAM memory address set to read from
	while(bytes_read < read_count  )
	{
		I2C_send_buffer[0]= control_byte_enable;//put in control port as a read function as previously found display needs enable first
		I2C_send_buffer[1]= 0xFF ;//put data port for read by sending 1's the data port now useable as read port
		HAL_I2C_Master_Transmit(&hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//activate the read
		HAL_Delay(1);// may be needed
		HAL_I2C_Master_Receive(&hi2c1, (device_add << 1), I2C_read_buf, 2, 200);//read both ports

		line_array.array[bytes_read] = I2C_read_buf[1];// store the 8 bit data port byte read
		HAL_Delay(10);
		I2C_send_buffer[0]= control_byte_no_enable;// toggle control port to diabale display enable
		bytes_read++;//increment the counter for bytes read
		HAL_I2C_Master_Transmit(&hi2c1, (device_add <<1), I2C_send_buffer, 2, 200);//de_activate the read
	}
	hd44780_set_line(0x21, 1, 0, false);
	hd44780_write_bulk_bytes(0x21,line_array.array , 5);
	return(line_array);
}

void hd44780_cursor_state(uint8_t device_add,bool display ,bool cursor, bool blink)
{
    uint8_t cursor_setting = DISPLAY_OFF;//everything is off
    cursor_setting = bit_modify(cursor_setting,HD_DISPLAY_BIT,display);//display on/off
    cursor_setting = bit_modify(cursor_setting,HD_CURSOR_BIT,cursor);//cursor on/off
    cursor_setting = bit_modify(cursor_setting,HD_BLINK_BIT,blink);//blink on
    hd44780_write_instruction(device_add, cursor_setting);// delay is 1ms actual 37us
}

/**
   * @brief    Initialises the PCF8575
   * @param BackLight
  * @param   (slave_add) hardware device address of slave
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_init_8bit(uint8_t device_add, _Bool BackLight)
{
	back_light_state = BackLight;
	uint8_t instruction_code;
	//instruction_code=  DISPLAY_OFF;
	instruction_code = FUNCTION_SET;//FUNCTION_SET = 0b001/DL/N/F/00
	instruction_code = bit_modify(instruction_code, HD_DL, 1);//DL = 1; 8-bit interface data
	instruction_code = bit_modify(instruction_code, HD_N, 1);// N = 1: 2 lines, N = 0:  1 line
	instruction_code = bit_modify(instruction_code, HD_F, 1);// F = 1: 5 x10 dots, F = 0:  5 x 8 dots
	hd44780_write_instruction(device_add,instruction_code);
	instruction_code = ENTRY_MODE_SET;// ENTRY_MODE_SET = 0b00000100
	instruction_code = bit_modify(instruction_code, HD_ID, 1);// I/D = 1: Increment  I/D = 0: Decrement
	instruction_code = bit_modify(instruction_code, HD_S, 0); //S = 1: Accompanies display shift
	hd44780_write_instruction(device_add,instruction_code);
	hd44780_write_instruction(device_add, CLEAR_DISPLAY);
	//hd44780_clr_disp(device_add);
	hd44780_cursor_state(device_add,HD_DISPLAY_ON, HD_CURSOR_ON, HD_BLINK_ON);//set for start up.
}
/**
   * @brief    Sets DDRAM address start address.
   * 				DDRAM data is sent and received after this setting.
   * 				Sets the display memory area associated with start of line 0 to line 3
   * 				calculates the cursor position to start from on a line from 0 to 19
   * 				Clears the line if required from cursor position to line end 19
  * @param   (slave_add) hardware device address of slave
  * @param (line_no) 0 to 3 not trapped
  * @param (cursor_position) 0 to 19 not trapped
  * @param (clear_line) true (1) or false(0) clears from cursor position to position 19 not trapped
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_set_line(uint8_t device_add, uint8_t line_no, uint8_t cursor_position, bool clear_line)//sets line to 0 to 3 ie 1 to 4
{
    uint8_t loop;
    uint8_t pad_count = 0;
    char space_padding[20];
    uint8_t ddram = SET_DDRAM_ADDRESS;// command value for moving cursor in display ram
    switch (line_no)
    {
        case 0 : ddram = ddram | HD_LINE0;// or first byte of line 0
        break;
        case 1 : ddram = ddram | HD_LINE1;// or first byte of line 1
        break;
        case 2 : ddram = ddram | HD_LINE2;// or first byte of line 2
        break;
        case 3 : ddram = ddram | HD_LINE3;// or first byte of line 3
        break;
    };
    ddram = ddram + cursor_position;// add where the cursor should be on line 0- 19
    hd44780_write_instruction(device_add, ddram);
    if (clear_line == true)
    {
        for(loop = cursor_position; loop <=(HD_DISPLAY_WIDTH - cursor_position ); loop++)
        {
          hd44780_write_single_byte(device_add,0x20);// write space to clear line
        	//space_padding[loop] = 0x20;
        	//pad_count++;
        }
       // hd44780_write_bulk_data(device_add, space_padding, pad_count  );


    }
    //HAL_Delay(1);
    hd44780_write_instruction(device_add, ddram);//Put the cursor back where we want it again

}
/**
   * @brief    Writes a single ASCII character at a specific line and cursor position
  * @param   (slave_add) hardware device address of slave
  * @param  A single character value
  * @param (line_no) 0 to 3 not trapped
  * @param (cursor_position) 0 to 19 not trapped
  * @param (clear_line) true (1) or false(0) clears from cursor position to position 19 not trapped
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_write_character_at(uint8_t device_add, char character,uint8_t line, uint8_t cursor,bool clear_line)
{
	char single_array[1];
	single_array[0] = character;
	single_array[1] = 0;
	hd44780_set_line(device_add,line,cursor,clear_line);
	hd44780_write_bulk_bytes(device_add, single_array, 0);
	//hd44780_write_data(device_add, character);
}
/**
   * @brief    Writes an ASCII message starting at the line and cursor position previously set not trapped
  * @param  (slave_add) hardware device address of slave
  * @param  ASCII message[] message length not trapped
  * @param (line_no) 0 to 3 not trapped
  * @param (cursor_position) 0 to 19 not trapped
  * @param (clear_line) true (1) or false(0) clears from cursor position to position 19 not trapped
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_write_message(uint8_t device_add,char message[])
{
    hd44780_write_bulk_bytes(device_add, message,0);
}

/**
   * @brief    Writes an ASCII message at a specific line and cursor position
  * @param   (slave_add) hardware device address of slave
  * @param  (message[]) char string or char array
  * @param (line_no) 0 to 3 not trapped
  * @param (cursor_position) 0 to 19 not trapped
  * @param (clear_line) true (1) or false(0) clears from cursor position to position 19 not trapped
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_write_message_at(uint8_t device_add,char message[], uint8_t line, uint8_t cursor, bool clear_line)
{
    hd44780_set_line(device_add,line,cursor,clear_line);
    hd44780_write_bulk_bytes(device_add, message,STRING);
}

/**
   * @brief    Writes an ASCII character at the last line and position held in the HD44780 display not trapped
  * @param   (slave_add) hardware device address of slave
  * @param  (character) any ASCII character in the fix character rom and the CGRAM
   * @retval no return value as yet
   * last updated 13/04/2026
   */
void hd44780_write_character(uint8_t device_add,char character)
{
	//char single_array[1];
	//single_array[0]= character;
	//single_array[1]= 0;
	//hd44780_write_bulk_data(device_add, single_array, 1);
	hd44780_write_single_byte(device_add, character);
}

void hd44780_clr_disp(uint8_t device_add)
{
	hd44780_set_line(device_add,0,0,true);
	hd44780_set_line(device_add,1,0,true);
	hd44780_set_line(device_add,2,0,true);
    hd44780_set_line(device_add,3,0,true);
}

/**
   * @brief    Writes an CUSTOM character to the CGRAM address 0 to 7 not trapped
  * @param   (device_add) hardware device address of slave
  * @param   add_of_char the calling address of the custom char in CGRAM
  * @param   patten 5its wide by 8 bytes deep
  * @param  temp used to calculate place in CGRM memory for each custo character
   * @retval no return value as yet
   * last updated 19/06/2026
   */
void hd44780_custom_char(uint8_t device_add,uint8_t add_of_char, char patten[], uint8_t patten_size)
{
	uint8_t cgram_char_add = (SET_CGRAM_ADDRESS) | (8 * add_of_char);
	 // base instruction and address in array field of CGRAM
	hd44780_write_instruction(device_add,cgram_char_add);
	hd44780_write_bulk_bytes(device_add, patten, patten_size);

}
void HD44780_new_text_page(uint8_t device_add)
{
    hd44780_clr_disp(device_add ); // cursor to top left
    text_count = 0;
    text_line = 0;
}
/*
void HD44780_write_text( uint8_t device_add,char ch)
{
    switch (text_line)
    {
        case 0 :
        {
            text_line_0[text_count] = ch;//put line into memory
            hd44780_write_character_at(device_add, character, line, cursor, clear_line)
            hd44780_write_character_at(device_add, character, line, cursor, clear_line)
            hd44780_write_character_at(ch,0,text_count,true);
            text_count++;
            if(text_count >=19)
            {
                text_count = 0;
                 text_line = 1;
            }
            break;
        }
         case 1 :
        {
            text_line_1[text_count] = ch;//put line into memory
            hd44780_write_character_at(ch,1,text_count,true);
            text_count++;
            if(text_count >=19)
            {
                text_count = 0;
                text_line = 2;
            }
            break;
        }
         case 2 :
        {
            text_line_2[text_count] = ch;//put line into memory
            hd44780_write_character_at(ch,2,text_count,true);
            text_count++;
            if(text_count >=19)
            {
                text_count = 0;
                text_line = 3;
            }
            break;
        }
         case 3 :
        {
            text_line_3[text_count] = ch;//put line into memory
            hd44780_write_character_at(ch,3,text_count,true);
            text_count++;
            if(text_count >=19)
            {
                text_count = 0;
                text_line = 4;
            }
            break;
        }
         case 4 :
        {
            strcpy(text_line_0,text_line_1);
            hd44780_set_line(0,0,true);
            hd44780_write_message(text_line_0);
            strcpy(text_line_1,text_line_2);
            hd44780_set_line(1,0,true);
            hd44780_write_message(text_line_1);
            strcpy(text_line_2,text_line_3);
            hd44780_set_line(2,0,true);
            hd44780_write_message(device_add,text_line_2);
            text_line_3[text_count] = ch;//put line into memory
            hd44780_write_character_at(ch,3,text_count,true);
            text_count++;
            text_line = 3;
            break;
        }

    }
}
void HD44780_string_as_text(char text_str[], uint8_t line,uint8_t cursor)
{
    uint16_t length;
    uint16_t loop;
    text_line = line;
    text_count = cursor;
    hd44780_set_line(line,cursor,true);
    length = strlen(text_str);
    for(loop = 0; loop <= length -1; loop++)
    {
        hd44780_write_text(text_str[loop]);
    }
}

*/

int bit_set(int x, unsigned char position)
 {
    int mask =0;
    mask = 1 << position;
    return x | mask;
 }

int bit_clear(int x, unsigned char position)
 {
    int mask = 0;
    mask = 1 << position;
    return x & ~mask;
 }

int bit_modify(int x, unsigned char position, bool new_state)
 {
    int mask = 0;
    int state;

    mask = 1 << position;
    if (new_state == 1) state = 1; else state = 0;// relies on true = 1 and false = 0
    return (x & ~mask) | (-state & mask);
 }

int bit_flip(int x, unsigned char position)
 {
    int mask = 0;
    mask = 1 << position;
    return x ^ mask;
 }

_Bool bit__is_it_set(int x, unsigned char position)
 {
    x >>= position;
    return (x & 1) != 0;
 }
