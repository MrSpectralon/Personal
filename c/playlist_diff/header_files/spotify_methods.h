#pragma once

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev
#include <cjson/cJSON.h>


typedef struct {
  char *data;
  size_t size;

} ResponseBuffer;

typedef struct {
  char *token;
  char *type;
  int duration;
  struct tm *time_recieved;
    
} SpotifyAccess;


SpotifyAccess* spotify_access_init(char* auth_reply);

size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);

char* getAuthTokenSpotify(char* clientID, char* clientSecret);



void getAuthKey(CURL *opts, CURLcode res);
void print_spotify_access(SpotifyAccess *access_obj); 
void spotify_access_delete(SpotifyAccess *access_obj);