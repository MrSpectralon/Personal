#pragma once

#include "./helper_methods.h"

#include <curl/curl.h>
#include <string.h>

typedef struct {
  char *data;
  size_t size;

} ResponseBuffer;


size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);


void get_auth_Key(CURL *opts, CURLcode res);

char* curl_get_request(const char* authorization, const char* requestURL);

char* curl_post_request(const char* destination_url, const char* content_type,
    const char* post_field, const long request_len);