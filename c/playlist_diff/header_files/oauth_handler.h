#pragma once

//Only in this header file because it includes ctime for my LSP to stop complaining.
#include <curl/curl.h> 

#include <cjson/cJSON.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum{
	SPOTIFY,
	YOUTUBE
} Service;


typedef struct {
	char *token;
	char *type;
	int duration;
	Service service;
	struct tm *time_recieved;
    
} OauthAccess;


OauthAccess *oauth_access_init(const char* auth_reply, Service service);
void oauth_access_delete(OauthAccess** access_obj);

void oauth_access_print (OauthAccess *access_obj);