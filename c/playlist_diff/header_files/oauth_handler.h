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

/**
 * struct OauthAccess - Represents an OAuth access token with associated metadata.
 *
 * This struct holds information related to an OAuth access token, including
 * token details, token type, associated playlist, token duration, service
 * provider, and the time the token was received. It is intended to manage
 * access tokens for a streaming service or API with specific access and metadata
 * requirements.
 *
 * Members:
 * @token:       The OAuth access token string, used for authenticating API requests.
 * @type:        The type of the token (e.g., "Bearer"), indicating its usage or authorization level.
 * @playlist:    The playlist ID or name associated with the token (if applicable).
 * @duration:    The duration (in seconds) that the token is valid for, after which it expires.
 * @service:     Enum or struct representing the service provider (e.g., Spotify, Apple Music).
 * @time_recieved: Pointer to a `struct tm` representing the date and time when the token was received,
 *                 used to calculate expiration based on the `duration` field.
 */
typedef struct {
	char *token;
	char *type;
	char *playlist;
	int duration;
	Service service;
	struct tm *time_recieved;
    
} OauthAccess;


OauthAccess *oauth_access_init(const char* auth_reply, Service service, const char* playlist_id);
void oauth_access_delete(OauthAccess** access_obj);

void oauth_access_print (OauthAccess *access_obj);
