#include "../header_files/helper_methods.h"

const char* SERVICES[] = {"Spotify", "YouTube Music"};


size_t write_callback (void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf)
{
    size_t total_size = size * nmemb;

    // Reallocate memory to fit the new data
    char *temp = realloc (res_buf->data, res_buf->size + total_size + 1);
    if (temp == NULL)
    {
        fprintf (stderr, "Failed to allocate memory.\n");
        return 0;		// Returning 0 will signal libcurl to abort the request
    }

    res_buf->data = temp;
    memcpy (&(res_buf->data[res_buf->size]), ptr, total_size);
    res_buf->size += total_size;
    res_buf->data[res_buf->size] = '\0';	// Null-terminate the string

    return total_size;
}


void remove_new_line (char *str)
{
    size_t len = strlen (str);
    if (len > 0 && str[len - 1] == '\n') 
    {
        str[len - 1] = '\0';
    }
}

int string_compare_64b (const char *str1, const char *str2)
{
    size_t len = strlen (str1);
    size_t len2 = strlen (str2);

    if (len != len2)
    {
        return 0;
    }
    size_t i;

    // Compare 8 bytes (64 bits) at a time as long as there are 64 or more bits left of the string.
    for (i = 0; i + sizeof (uint64_t) <= len; i += sizeof (uint64_t))
    {
        uint64_t *ptr1 = (uint64_t *) (str1 + i);
        uint64_t *ptr2 = (uint64_t *) (str2 + i);
        if (*ptr1 != *ptr2)
        {
            return 0;
        }
    }
    // Handle remaining bytes (if the string length is not a multiple of 8)
    for (; i < len; ++i)
    {
        if (str1[i] != str2[i])
        {
            return 0;
        }
    }
    return 1;
}


char* curl_request (const char *authorization, const char *requestURL)
{
    CURLcode ret;
    CURL *hnd;
    ResponseBuffer res_buf;
    struct curl_slist *slist1;
    res_buf.data = malloc (1);
    res_buf.size = 0;

    slist1 = NULL;
    slist1 = curl_slist_append (slist1, authorization);
    hnd = curl_easy_init ();

    curl_easy_setopt (hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt (hnd, CURLOPT_URL, requestURL);
    curl_easy_setopt (hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt (hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt (hnd, CURLOPT_USERAGENT, "curl/8.5.0");
    curl_easy_setopt (hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt (hnd, CURLOPT_HTTP_VERSION,
		      (long) CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt (hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt (hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt (hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, &res_buf);

    ret = curl_easy_perform (hnd);

    if (ret != CURLE_OK)
      {
	  fprintf (stderr, "curl_easy_perform() failed: %s\n",
		   curl_easy_strerror (ret));
      }

    curl_easy_cleanup (hnd);
    curl_slist_free_all (slist1);
    hnd = NULL;
    slist1 = NULL;
    return res_buf.data;
}

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

    memset ((*access_obj)->token, '\0', strlen ((*access_obj)->token));
    free 
    ((*access_obj)->token);
    free ((*access_obj)->type);
    free ((*access_obj)->time_recieved);
    free ((*access_obj));
}


char* base64url_encode(const char* string, const size_t len)
{
    //Base 64 encoded string.
    char* encoded = NULL;
    //Base 64 URL safe table. Base 64 decimal value = index value 
    char b64urls_table[] = "ABCDEFGHIJKLMNOPQRSTUZWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    
    //Decimal value for lookup table.
    int8_t val = 0;
    //Number of 6 bit chunks
    uint64_t num_o_chunks = (len * 8) / 6;
    //Counter for which base64 character is being worked on
    uint64_t chunk_pos = 0;
    //Counter for position in the base64 bit index.
    uint8_t bit_chunk_pos = 5;

    encoded = calloc(num_o_chunks + 1, sizeof(char));
    if(encoded == NULL)
    {
        fprintf(stderr, "Error allocating memory for b64 encoded string.\n");
        goto cleanup;        
    }
    
    //Iterating through all characters in string
    for (size_t c = 0; c < len; c++)
    {

        //Iterating throug all bits in current character.
        for (int8_t b = 7; b >= 0; b--)
        {
            //Checking if current bit has value 1.    
            if(string[c] >> b & 1)
            {
                //Adding 2^bit_chunk_pos to the base64 value
                val += (uint8_t)1 << bit_chunk_pos;
            }

            //Check if we need to go to the next 6 bit chunk.
            if (bit_chunk_pos == 0)
            {
                //Appends current character and resets.
                encoded[chunk_pos] = b64urls_table[val];
                chunk_pos++;
                bit_chunk_pos = 5;
                val = 0;
            }
            else
            {
                //Checking if on last character iteration
                if (c == (len - 1) && b == 0)
                {
                    //Adding next encoded character if character iterations ended before finishing next complete base64 character.
                    encoded[chunk_pos] = b64urls_table[val];
                }
                bit_chunk_pos--;
            }
        }
    }
    encoded[len + 1] = '\0';
    return encoded;

    cleanup:
    free(encoded);
    return NULL;
}


char* hmac256sha_encode(const char* string, const size_t len)
{

}

char* __sha256();
