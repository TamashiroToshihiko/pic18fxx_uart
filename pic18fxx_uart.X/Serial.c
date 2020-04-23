/*
 * File:   Serial.c
 * Author: ttama
 *
 * Created on 2020/02/08, 10:10
 */


#include <xc.h>
#include "system.h"

static char buffer_over     = 0;
static char buffer_over_cnt = 0;
static char send_cnt        = 0;

/*
 * Function call by set_up()
 */
void Serial_init(void)
{
    Serial.begin     = begin;
    Serial.available = available;
    Serial.print     = print;
    Serial.println   = println;
    Serial.read      = read;
    Serial.write     = write;
    Serial.variable.read_data_length = 0;
    Serial.variable.send_data_length = 0;
    Serial.variable.read_buffer_select = buffer_select_A;
    Serial.variable.send_buffer_select = buffer_select_A;
    if(INTCONbits.GIE == 0)
    {
        INTCONbits.GIE  = 1;
        INTCONbits.PEIE = 1;
    }
    PIR1bits.RCIF = 0;
    PIR1bits.TXIF = 0;
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 1;
    TX_PIN_INT();
    RX_PIN_INIT();
}

static void begin(long baud_rate)
{
    unsigned short int brg;
    BAUDCON = 0x00;
    BAUDCONbits.BRG16 = 1;
    TXSTA   = 0b00100100;
    RCSTA   = 0b10010000; 
    brg = (unsigned short int)(_XTAL_FREQ/baud_rate/4) - 1;
    SPBRGH  = (unsigned char)(brg>>8);
    SPBRG   = (unsigned char)(brg & 0xFF);
}

static void write(char data)
{
    while(!TXSTAbits.TRMT);
    TXREG = data;
}

static void print(char *data_buf)
{
    static char cnt = 0;
    static char pp;
    while(Serial.variable.send_status[buffer_select_A] == true && Serial.variable.send_status[buffer_select_B] == true);
    if(Serial.variable.send_buffer_select == buffer_select_A)
    {
        pp = Serial.variable.send_buffer_select;
        Serial.variable.send_buffer_select = buffer_select_B;
    }
    else
    {
       pp = Serial.variable.send_buffer_select;
       Serial.variable.send_buffer_select = buffer_select_A;   
    }
    cnt = 0;
    while(*data_buf != 0x00)
    {
        Serial.variable.send_buffer[pp][cnt++] = *data_buf++;
    }
    Serial.variable.send_data_length = cnt-1;
    Serial.variable.send_status[pp] = true;
    PIE1bits.TXIE = 1;
    
}

static void println(char *data_buf)
{
    Serial.print(data_buf);
    Serial.write(0x0D);
    Serial.write(0x0A);
}

static int  available(void)
{
    if(Serial.variable.read_status == true)
    {
        return (int)Serial.variable.read_data_length + 1;
    }
    return -1;
}
static char read(char *read_buf,char read_len)
{
    static char i,p;
    if(Serial.variable.read_status == false || Serial.variable.read_data_length<read_len)
    {
        return 0;
    }
    while(PIR1bits.RCIF);
    Serial.variable.read_data_length   = 0;
    if(Serial.variable.read_buffer_select == buffer_select_A)
    {
        if(buffer_over == true)
        {
            buffer_over = false;
            p = buffer_select_B;
        }
        else
        {
            p = buffer_select_A;
        }
        Serial.variable.read_buffer_select = buffer_select_B;
        
    }
    else
    {
        if(buffer_over == true)
        {
            buffer_over = false;
            p = buffer_select_A;
        }
        else
        {
            p = buffer_select_B;
        }
        Serial.variable.read_buffer_select = buffer_select_A;
        p = buffer_select_B;
    }
    
    for(i=0;i<read_len;i++)
    {
        *read_buf++ = Serial.variable.read_data[p][i]; 
        Serial.variable.read_data[p][i] = 0x00;
    }
    return 1;
}
void _UART_INTERRUPT_TASK(void)
{
    if(PIR1bits.RCIF)
    {
        PIR1bits.RCIF = 0;
        Serial.variable.read_status = true;
        if(Serial.variable.read_data_length>(MAX_DATA_BUF-1))
        {
            buffer_over = true;
            buffer_over_cnt = Serial.variable.read_data_length;
            Serial.variable.read_data_length = 0;
            if(Serial.variable.read_buffer_select == buffer_select_A)
            {
                Serial.variable.read_buffer_select = buffer_select_B;
            }
            else
            {
                Serial.variable.read_buffer_select = buffer_select_A;
            }
        }
        if(Serial.variable.read_buffer_select == buffer_select_A)
        {
            Serial.variable.read_data[buffer_select_A][Serial.variable.read_data_length++] = RCREG;
        }
        else
        {
            Serial.variable.read_data[buffer_select_B][Serial.variable.read_data_length++] = RCREG;
        }
    }
    
    if((PIR1bits.TXIF && PIE1bits.TXIE) && (Serial.variable.send_status[buffer_select_A] == true || Serial.variable.send_status[buffer_select_B] == true))
    {
        PIR1bits.TXIF = 0;
        if(send_cnt > Serial.variable.send_data_length)
        {
            if(Serial.variable.send_buffer_select == buffer_select_A)
            {
                Serial.variable.send_status[buffer_select_A] = false;
            }
            else
            {
                Serial.variable.send_status[buffer_select_B] = false;
            }
            Serial.variable.send_data_length = 0;
            
            send_cnt = 0;
            PIE1bits.TXIE = 0;
            PIR1bits.TXIF = 0;
        }
        else if(Serial.variable.send_buffer_select == buffer_select_A)
        {
            TXREG = Serial.variable.send_buffer[buffer_select_B][send_cnt++];
        }
        else
        {
            TXREG = Serial.variable.send_buffer[buffer_select_A][send_cnt++];
        }
    }
}