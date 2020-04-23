/* 
 * File:   system.h
 * Author: ttama
 *
 * Created on 2020/02/08, 14:20
 */

#ifndef SYSTEM_H
#define	SYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif
#define _XTAL_FREQ 64000000
#include <xc.h>
#include "Serial.h"

#define set_up()                \
{                               \
    Initialize_task();          \
    Serial_init();              \
}

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTEM_H */

