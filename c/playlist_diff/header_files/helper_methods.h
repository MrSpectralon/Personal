#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

typedef enum{
	SPOTIFY,
	YOUTUBE
} Service;

typedef struct {
  char *data;
  size_t size;

} ResponseBuffer;


typedef struct {
	char *token;
	char *type;
	int duration;
	Service service;
	struct tm *time_recieved;
    
} OauthAccess;

size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);

/*
* Removes new line from end of string if `\n` is the last char of the string.
* Rmember to either use malloc before using this funcion, or do a +1 for correct data sizes when using strcpy or strcat etc.
*/
void remove_new_line(char* str);
int string_compare_64b(const char* str1, const char* str2);

char* curl_request(const char* authorization, const char* requestURL);

OauthAccess *oauth_access_init(const char* auth_reply, Service service);
void oauth_access_print (OauthAccess *access_obj);
void oauth_access_delete(OauthAccess** access_obj);

void get_auth_Key(CURL *opts, CURLcode res);


// homemade base64 urlsafe encoding
char* base64url_encode(const char* string, const size_t len);

char* hmac256sha_encode(const char* string, const size_t len);
