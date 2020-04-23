/*
 * File:   main.c
 * Author: ttama
 *
 * Created on July 2, 2019, 1:26 PM
 */


#include "system.h"

//------------------------------------
static void CLOCK_Initialize(void);
static void Initialize_task();
//-------------------------------------


/*
 *Function call by set_up()
 */
static void Initialize_task()
{
    //initialize 
    CLOCK_Initialize();
}
char data[10];

int main(void) {
    set_up();
    Serial.begin(9600);

    while(1)
    {
        if(Serial.available()>5)
        {
            Serial.read(data,5);
            Serial.print("READ = ");
            Serial.println(data);
            
        }
        Serial.println("hello world1!!");
        __delay_ms(1000);
    }
}
static void CLOCK_Initialize(void)
{
   //clock initialize
}