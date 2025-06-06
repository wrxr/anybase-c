// yo yo yo
// dec/bin/hex/oct converter
// by wrxr
// CLION 2025 CMAKE CMAKE-C-STANDARD 23
// ебите меня семеро

#pragma once

#include <stdint.h>

char* dtob(const int64_t); // Decimal to binary
                           // printf %s
int64_t btod(const char*); // Binary to decimal
                           // printf %lld or ld (compiler)

char* dtoh(const int64_t); // Decimal to hex
                           // printf %s
int64_t htod(const char*); // Hex to decimal
                           // printf %lld or ld (compiler)

char* dtoo(const int64_t); // Decimal to octal
                           // printf %s
int64_t otod(const char*); // Octal to decimal
                           // printf %lld or ld (compiler)