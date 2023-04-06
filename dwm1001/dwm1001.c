/*
 * dwm1001.c
 *
 * Created: 2023-03-20
 * Author: Kia Skretteberg
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dwm1001.h"

/************************************************************************/
/* Local Definitions (private functions)                                */
/************************************************************************/

// Read the uart channel for the specified value and output the return value to the buffer
// the expect_type matches a single byte from the datasheet 
// which is followed by a byte indicating the number of bytes in the response
// which is followed by the specified number of bytes
// the buff will contain those response bytes only
int read_value(uint8_t expect_type, uint8_t * buff);

// Parse out a coordinate (x, y, z) as a 32 bit integer, starting from the startIdx of the buff, made up of individual 4 bytes
long read_coord(uint8_t * buff, int startIdx);


/************************************************************************/
/* Header Implementation                                                */
/************************************************************************/

void dwm1001_init_communication(void)
{
    // Set up our UART with the required speed.
    uart_init(DWM1001_UART_ID, DWM1001_BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(DWM1001_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(DWM1001_RX_PIN, GPIO_FUNC_UART);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(DWM1001_UART_ID, false, false);

    // Set our data format
    uart_set_format(DWM1001_UART_ID, DWM1001_DATA_BITS, DWM1001_STOP_BITS, DWM1001_PARITY);

    // TODO: Is the FIFO needed?
}

struct DWM1001_Position dwm1001_request_position(void)
{
    uint8_t errBuff[20];
    uint8_t dataBuff[200];
    uint8_t data2Buff[200];
    uint8_t error;
    char coordBuff[200];
    struct DWM1001_Position coords;
    // dwm_loc_get      see 5.3.10
    uart_putc_raw(DWM1001_UART_ID, 0x0C);
    uart_putc_raw(DWM1001_UART_ID, 0x00);

    //check if there's an error in the command
    error = read_value(0x40, errBuff);
    //TODO: if buff is not empty, that's an error as well
    if(!error) 
    {
        // get the device's position
        error = read_value(0x41, dataBuff);

        if(!error) 
        {
            // first 4 bytes are x, next 4 are y, next 4 are z, last 1 is quality(?)
            // bytes represent 32 bit integer (measuring mm)
            // example: 0x08 0x00 0x00 0x00    0x0B 0xFF 0xFF 0xFF    0x4C 0x00 0x00 0x00    0x00
            // bytes come in reverse order (LSByte first)
            // ~ x = -0.06, y = -0.03, z = 0.08
            coords.x = read_coord(dataBuff, 0);
            coords.y = read_coord(dataBuff, 4);
            coords.z = read_coord(dataBuff, 8);
            // get the position to the anchors (not needed if home is 0,0,0 but may be beneficial)
            
            error = read_value(0x49, data2Buff);
        }
    }
    
    return coords;
}

/************************************************************************/
/* Local  Implementation                                                */
/************************************************************************/

int read_value(uint8_t expect_type, uint8_t * buff)
{
    unsigned char count = 0;
    // max number of bytes to be read, at most is 253
    unsigned char maxLength = 253;
    
    // while there's data to read and we haven't read the end of this value (read all bytes indicated in value)
    while(uart_is_readable(DWM1001_UART_ID) && count < maxLength + 2)
    {
        uint8_t c = uart_getc(DWM1001_UART_ID);
        char buffer[20];
        sprintf(buffer, "\nread_value: %x", c);
        printf(buffer);
        ++count;
        if(count == 1)
        {
            // if the type doesn't match the expected type, throw an error!
            if(c != expect_type)
                return -1;
        }
        // grab the length of how many bytes to receive next
        else if(count == 2)
        {
            maxLength = c;
        }
        else
        {
            *buff = c;
            ++buff;
        }
    }
    unsigned char buffer[200];
    sprintf(buffer, "\nread_value final: %x", buff);
    printf(buffer);

    return 0;
}

long read_coord(uint8_t * buff, int startIdx)
{
    char buffer[200];
    long value = buff[startIdx];
    value += buff[startIdx+1]<<8;
    value += buff[startIdx+2]<<16;
    value += buff[startIdx+3]<<24;
    sprintf(buffer, "\nread_coord: %x", value);
    printf(buffer);
    return value;
}