#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


typedef unsigned char b512_t[64];

/*
* Removes new line from end of string if `\n` is the last char of the string.
* Remember to either use malloc before using this funcion, or do a +1 for correct data sizes when using strcpy or strcat etc.
*/
void remove_new_line(char* str);

uint32_t bit_rotate_right(const uint32_t data, const int rotations);

//Homemade strcmp that compares 8 bytes at a time - as far as possible.
int string_compare_64b(const char* str1, const char* str2);

// homemade base64 urlsafe encoding
char* base64url_encode(const char* string, const size_t len);


unsigned char* sha256_encode(const char* string, const size_t len);


unsigned char* hmac_sha256(const char* key, const size_t key_s, const char* msg, const size_t msg_s);