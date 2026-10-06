#ifndef PRINT_H_
#define PRINT_H_

#include <stdint.h>
#include <stdbool.h>

char charToHex( char c);
void intToString(int32_t n, uint8_t numDigits, char *s);
void unsignedToStringZero(uint32_t num, uint8_t numDigits, char *s);
void unsignedToString(uint32_t num, uint8_t numDigits, char *s);
void unsignedToStringX(uint32_t num, uint8_t numDigits, char *s);
const char* stringToUnsignedInternal( const char* P, uint32_t* V);
bool stringToUnsigned( const char* P, uint32_t* V);
bool stringToUnsignedX( const char* P, uint32_t* V);

#endif