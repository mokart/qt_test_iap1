#ifndef MYCRC16_H
#define MYCRC16_H

#include <stdio.h>
#include <stdlib.h>

#define USHORT unsigned short int
#define uchar unsigned char
#define uint unsigned int


USHORT fenduanCRC( unsigned char *puchMsg,unsigned int usDataLen,USHORT temp);


#endif

