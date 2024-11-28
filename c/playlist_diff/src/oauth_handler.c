#include "../header_files/oauth_handler.h"
#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* SERVICES[] = {"Spotify", "YouTube Music"};

/**
 * In hindsight this function does way more than just simply initialize the struct...
 * This function parces json and adds the findings into a OauthAccess object and returns it.
 * Returns NULL if an error occurs.
 */
OauthAccess* oauth_access_init (const char* auth_reply, Service service, const char* playlist_id)
{
    cJSON* scope = NULL;
    cJSON* refresh_token = NULL;
    cJSON* access_token = NULL;
    cJSON* token_type = NULL;
    cJSON* expiration_time = NULL;

    OauthAccess* data = NULL; 
    data = malloc(sizeof(OauthAccess));
    if (data == NULL) 
	{
		goto cleanup;
    }
    data->token = NULL;
    data->type = NULL;
    data->playlist = NULL;
    data->time_recieved = NULL;
	data->scope = NULL;
	data->refresh_token = NULL;
    cJSON* json = cJSON_Parse (auth_reply);
    if (json == NULL)
    {
		fprintf (stderr, "An error occured when parcing Spotify authentication data.\n");
		cJSON_Delete (json);
		return NULL;
    }


    cJSON* error = cJSON_GetObjectItem (json, "error");
    if (error != NULL)
    {
        cJSON *error_description = cJSON_GetObjectItem (json, "error_description");
        printf("Authentication error:\n");
        printf("\tError: %s\n", error->valuestring);
        printf("\tDescription: %s\n", error_description->valuestring);
        goto cleanup;
    }

    access_token = cJSON_GetObjectItem (json, "access_token");
    token_type = cJSON_GetObjectItem (json, "token_type");
    expiration_time = cJSON_GetObjectItem (json, "expires_in");


    if (access_token == NULL)
    {
		fprintf (stderr, "No access token found.\n");
        goto cleanup;
    }

    if (token_type == NULL)
    {
        fprintf (stderr, "No token type found.\n");
        goto cleanup;
    }
        

    if (expiration_time == NULL)
    {
		fprintf (stderr, "Got no expiration time for authorization key.\n");
		goto cleanup;
    }

    if (service == YOUTUBE) 
	{
		scope = cJSON_GetObjectItem(json, "scope");
		refresh_token = cJSON_GetObjectItem(json, "refresh_token");
		if (scope == NULL || refresh_token == NULL) 
		{
			fprintf (stderr, "Got no scope or refresh token from youtube.\n");
			goto cleanup;
		}
    }

    data->playlist = strdup(playlist_id);
    data->service = service;
    data->token = strdup (access_token->valuestring);
    data->type = strdup (token_type->valuestring);
    data->duration = expiration_time->valueint;
    data->time_recieved = malloc (sizeof (struct tm));

    data->scope = (scope == NULL) ? NULL : strdup(scope->valuestring);
    data->refresh_token = (refresh_token == NULL) ? NULL : strdup(refresh_token->valuestring);

    if (data->time_recieved == NULL 
	|| data->playlist == NULL 
	|| data->type == NULL 
	|| data->token == NULL)
    {
	goto cleanup;
    }

    time_t now = time (NULL);
    *(data->time_recieved) = *localtime (&now);
    cJSON_Delete(json);
    return data;

    cleanup:
    free(data->playlist);
    free(data->type);
    free(data->token);
    free(data->time_recieved);
    free(data->refresh_token);
    free(data->scope);
    cJSON_Delete(json);
    return NULL;
}


/**
 * Prints whatever is in the OauthAccess object in a somewhat presentable manner.
 */
void oauth_access_print (OauthAccess *access_obj)
{
    char time_str[100];
    strftime (time_str, sizeof (time_str), "%Y-%m-%d %H:%M:%S",
	      access_obj->time_recieved);
    printf ("Access token:\t%s\n", access_obj->token);
    printf ("Grant type:\t%s\n", access_obj->type);
    printf ("Duration:\t%i\n", access_obj->duration);
    printf ("Time recieved:\t%s\n", time_str);
    printf ("Service: %s\n", SERVICES[access_obj->service]);
	if (access_obj->service == YOUTUBE) {
		printf("API Scope: %s\n", access_obj->scope);
		printf("Refresh token: %s\n", access_obj->refresh_token);
	}
    printf ("Playlist ID: %s\n", access_obj->playlist);
}


/**
 * Frees all memory related to the OauthAccess object and deletes it. 
 */
void oauth_access_delete (OauthAccess** access_obj)
{
    if (*access_obj == NULL) return;
    memset((*access_obj)->token, '\0', strlen((*access_obj)->token));
    free((*access_obj)->token);
    free((*access_obj)->type);
    free((*access_obj)->playlist);
    free((*access_obj)->time_recieved);
    free((*access_obj)->scope);
    free((*access_obj)->refresh_token);
    free(*access_obj);
    *access_obj = NULL;
}
