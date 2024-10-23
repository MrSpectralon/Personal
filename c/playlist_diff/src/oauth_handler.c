#include "../header_files/oauth_handler.h"

const char* SERVICES[] = {"Spotify", "YouTube Music"};


OauthAccess* oauth_access_init (const char *auth_reply, Service service)
{
    cJSON *json = cJSON_Parse (auth_reply);
    if (json == NULL)
	{
		fprintf (stderr,
			"An error occured when parcing Spotify authentication data.\n");
		cJSON_Delete (json);
		return NULL;
	}


    cJSON *error = cJSON_GetObjectItem (json, "error");
    if (error != NULL)
    {
        cJSON *error_description = cJSON_GetObjectItem (json, "error_description");
        printf("Authentication error:\n");
        printf("\tError: %s\n", error->valuestring);
        printf("\tDescription: %s\n", error_description->valuestring);
        goto cleanup;
    }

    cJSON *access_token = cJSON_GetObjectItem (json, "access_token");
    cJSON *token_type = cJSON_GetObjectItem (json, "token_type");
    cJSON *expiration_time = cJSON_GetObjectItem (json, "expires_in");

    OauthAccess *data = malloc (sizeof (OauthAccess));

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

    data->service = service;
    data->token = strdup (access_token->valuestring);
    data->type = strdup (token_type->valuestring);
    data->duration = expiration_time->valueint;
    data->time_recieved = malloc (sizeof (struct tm));
    if (data->time_recieved != NULL)
	{
		time_t now = time (NULL);
		*(data->time_recieved) = *localtime (&now);
	}

    cJSON_Delete(json);
    return data;
	cleanup:

    cJSON_Delete(json);
	return NULL;
}


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
}

void oauth_access_delete (OauthAccess** access_obj)
{
    if (*access_obj == NULL) return;

    memset ((*access_obj)->token, '\0', strlen((*access_obj)->token));
    free ((*access_obj)->token);
    free ((*access_obj)->type);
    free ((*access_obj)->time_recieved);
    free (*access_obj);
    *access_obj = NULL;
}