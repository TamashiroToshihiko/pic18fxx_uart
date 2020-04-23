/* 
 * File:   uart.h
 * Author: ttama
 *
 * Created on 2020/02/08, 10:10
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif


    
#define MAX_DATA_BUF (65)    
#define buffer_select_A (0)
#define buffer_select_B (1)
#define true   (1)
#define false (0)    

void Serial_init(void);
    
static void begin(long baud_rate);

static void write(char data);

static void print(char *data_buf);

static void println(char *data_buf);

static int  available(void);

static char read(char *read_buf,char read_len);

void _UART_INTERRUPT_TASK(void);

typedef struct __Serial
{
    void (*begin)(long);
    void (*write)(char);
    void (*print)(char *);
    void (*println)(char *);
    int  (*available)(void);
    char (*read)(char *,char);
    struct
    {
        char read_data[2][MAX_DATA_BUF];
        char read_data_length;
        char read_buffer_select;
        char read_status;
        char send_buffer[2][MAX_DATA_BUF];
        char send_buffer_select;
        char send_data_length;
        char send_status[2];
    }variable;
}_Serial;
  
_Serial Serial;

#define RX_PIN_INIT() (TRISCbits.TRISC7 = 1 )
#define TX_PIN_INT()  (TRISCbits.TRISC6 = 0 )


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

