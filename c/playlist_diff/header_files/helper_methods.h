#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <curl/curl.h>
typedef struct {
  char *data;
  size_t size;

} ResponseBuffer;


size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);

/*
* Removes new line from end of string if `\n` is the last char of the string.
* Rmember to either use malloc before using this funcion, or do a +1 for correct data sizes when using strcpy or strcat etc.
*/
void removeNewline(char* str);

char* curl_request(char* authorization, char* requestURL);
int string_compare_64b(const char* str1, const char* str2);