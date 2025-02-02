/* utils.h */
#pragma once
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#pragma GCC diagnostic ignored "-Wformat-truncation="
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic push

#include <stdio.h>

typedef unsigned char int8;
typedef unsigned short int int16;
typedef unsigned int int32;

/* C */
void copy(int8*,int8*,int16);

/* Z */
void zero(int8*,int16);

/* P */
void printhex(int8*,int16,int8);