#include "../header_files/spotify_methods.h"


size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf) {
    size_t total_size = size * nmemb;

    // Reallocate memory to fit the new data
    char *temp = realloc(res_buf->data, res_buf->size + total_size + 1);
    if(temp == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0; // Returning 0 will signal libcurl to abort the request
    }

    res_buf->data = temp;
    memcpy(&(res_buf->data[res_buf->size]), ptr, total_size);
    res_buf->size += total_size;
    res_buf->data[res_buf->size] = '\0'; // Null-terminate the string

    return total_size;
}

char* getPlaylistContentSpotify(char* playlistID, SpotifyAccess* ad){

  ResponseBuffer res_buf;
  res_buf.data = malloc(1);
  res_buf.size = 0;
  
  CURLcode ret;
  CURL *hnd;
  char* requestURL; 
  char* authorization;
  int g_type_len = (int)strlen(ad->type); 
  struct curl_slist *slist1;


  //Preparing authentification
  char auth[] = "Authorization: ";
  char type[g_type_len+1];
  type[0] = '\0'; 
  strcat(type, ad->type);  
  type[g_type_len] = ' ';

  int auth_length = strlen(auth) + g_type_len+1 + strlen(ad->token);
  authorization = calloc(auth_length, sizeof(char));
  strcat(authorization, auth);
  strcat(authorization, type);
  strcat(authorization, ad->token);

  //Preparing approperiate URL for fetching playlist data. 

  char url_api_dest[] = "https://api.spotify.com/v1/playlists/";
  // At the moment hardcoded for Norwegian music licensing. (market=NO)
  char field_options[] = "?market=NO&fields=name%2C+description%2C+uri%2C+tracks%28items%28track%28name%2C+external_urls%2C+album%28name%29%2C+artists%28name%29%29%29%29";
  
  
  int url_length = strlen(url_api_dest) + strlen(field_options) + strlen(playlistID);
  requestURL = calloc(url_length, sizeof(char));
  strcat(requestURL, url_api_dest);
  strcat(requestURL, playlistID);
  strcat(requestURL, field_options);
  
  printf("Auth string:\n%s\n", authorization);
  printf("\nRequest URL:\n%s\n", requestURL);
  // goto end;
  
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, authorization); 
  hnd = curl_easy_init();

  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, requestURL); 
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.5.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &res_buf);

  ret = curl_easy_perform(hnd);

  if(ret != CURLE_OK)
	{
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
	}

  curl_easy_cleanup(hnd);
  curl_slist_free_all(slist1);
  free(requestURL);
  free(authorization);

  hnd = NULL;
  slist1 = NULL;
  requestURL = NULL;
  authorization = NULL;
  return res_buf.data;

  end:
  return NULL;
  
}


char* getAuthTokenSpotify(char* clientID, char* clientSecret)
{

  ResponseBuffer res_buf;
  res_buf.data = malloc(1);
  res_buf.size = 0;
  
  CURLcode ret;
  CURL *hnd;
  char* curlPostField; 
  struct curl_slist *slist1;
  
  char grantTypeAndClientID[] = "grant_type=client_credentials&client_id=";
  char cliS[] = "&client_secret=";
 
  int fieldSize = strlen(grantTypeAndClientID)
                         + strlen(clientID)
                         + strlen(cliS)
                         + strlen(clientSecret);
  
  curlPostField = calloc(fieldSize, sizeof(char));
  strcat(curlPostField, grantTypeAndClientID);
  strcat(curlPostField, clientID);
  strcat(curlPostField, cliS);
  strcat(curlPostField, clientSecret);

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: application/x-www-form-urlencoded");
  hnd = curl_easy_init();

  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, "https://accounts.spotify.com/api/token");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, curlPostField);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)fieldSize);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.5.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &res_buf);

  ret = curl_easy_perform(hnd);

  if(ret != CURLE_OK)
	{
    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(ret));
	}

  curl_easy_cleanup(hnd);
  hnd = NULL;
  free(curlPostField);
  curl_slist_free_all(slist1);
  slist1 = NULL;
  return res_buf.data;
}

SpotifyAccess* spotify_access_init(char* auth_reply){
  cJSON *json = cJSON_Parse(auth_reply);
  if(json==NULL){
    fprintf(stderr, "An error occured when parcing Spotify authentication data.\n");
    cJSON_Delete(json);
    return NULL;
  }

  cJSON *access_token = cJSON_GetObjectItem(json, "access_token");
  cJSON *token_type = cJSON_GetObjectItem(json, "token_type");
  cJSON *expiration_time = cJSON_GetObjectItem(json, "expires_in");

  SpotifyAccess *data = malloc(sizeof(SpotifyAccess));
  
  if(!access_token->valuestring || !token_type->valuestring || !expiration_time->valueint){
    fprintf(stderr, "Something wrong with access key.\n");

    printf("Token valuestring: %s\n", access_token->valuestring);
    printf("Token type valuestring: %s\n", token_type->valuestring);
    printf("Duration valuestring: %i\n", expiration_time->valueint);

    goto cleanup;
  }

  data->token = strdup(access_token->valuestring);
  data->type = strdup(token_type->valuestring);
  data->duration = expiration_time->valueint;
  data->time_recieved = malloc(sizeof(struct tm));
  if(data->time_recieved != NULL){
    time_t now = time(NULL);
    *(data->time_recieved) = *localtime(&now);
  }
  cleanup:

  return data;
}


void print_spotify_access(SpotifyAccess *access_obj){
  char time_str[100];
  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", access_obj->time_recieved);
  printf("Access token:\t%s\n", access_obj->token);
  printf("Grant type:\t%s\n", access_obj->type);
  printf("Duration:\t%i\n", access_obj->duration);
  printf("Time recieved:\t%s\n", time_str);
}


void spotify_access_delete(SpotifyAccess *access_obj){
  
  if(access_obj ==  NULL) return;
  memset(access_obj->token, '\0', sizeof(access_obj->token));
  free(access_obj->token);
  free(access_obj->type);
  free(access_obj->time_recieved);
  free(access_obj);
}

void parce_spotify_reply_data(char* data)
{
  cJSON *json = cJSON_Parse(data);
  if (json == NULL)
  {
    fprintf(stderr, "Error parsing JSON.\n");
    goto cleanup;
  }

  cJSON *tracks = cJSON_GetObjectItem(json, "tracks");
  if (tracks == NULL)
  {
    fprintf(stderr, "Error fetching 'tracks' object from json string.");
    goto cleanup;
  }

  cJSON *items = cJSON_GetObjectItem(tracks, "items");
  if (items == NULL)
  {
    fprintf(stderr, "Error fetching 'items' object from tracks object string.");
    goto cleanup;
  }

  cJSON *item = NULL;
  cJSON_ArrayForEach(item, items)
  {
    cJSON *track = cJSON_GetObjectItem(item, "track");
    if (track==NULL) continue;
    
    cJSON *track_name = cJSON_GetObjectItem(track, "name");
    cJSON *track_URL= cJSON_GetObjectItem(track, "name");

    cJSON *album = cJSON_GetObjectItem(track, "album");
    cJSON *album_name = cJSON_GetObjectItem(album, "name");

    cJSON *artists = cJSON_GetObjectItem(track, "artists");
    
    int arr_size = cJSON_GetArraySize(artists);
    for(int i =0; i < arr_size; i++)
    {
      cJSON *artist = cJSON_GetArrayItem(artist, i);
      
    }


  }

  cleanup:
  cJSON_Delete(json);
  cJSON_Delete(tracks);
  cJSON_Delete(items);

}