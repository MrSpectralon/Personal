#pragma once
#include <stdio.h>
#include <string.h>

/*
* Removes new line from end of string if `\n` is the last char of the string.
* Rmember to either use malloc before using this funcion, or do a +1 for correct data sizes when using strcpy or strcat etc.
*/
void removeNewline(char* str);
