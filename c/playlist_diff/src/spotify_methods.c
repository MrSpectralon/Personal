#include "../header_files/spotify_methods.h"

char* getPlaylistInfoSpotify(char* playlistID, SpotifyAccess* access_data)
{
  char* requestURL; 
  char* authorization;
  int g_type_len = (int)strlen(access_data->type); 

  //Preparing authentification
  char auth[] = "Authorization: ";
  char type[g_type_len+1];
  type[0] = '\0'; 
  strcat(type, access_data->type);  
  type[g_type_len] = ' ';

  int auth_length = strlen(auth) + g_type_len + 1 + strlen(access_data->token);
  authorization = malloc(auth_length);
  authorization[0] = '\0';
  strcat(authorization, auth);
  strcat(authorization, type);
  strcat(authorization, access_data->token);

  //Preparing approperiate URL for fetching playlist data. 
  char url_api_dest[] = "https://api.spotify.com/v1/playlists/";
  // At the moment hardcoded for Norwegian music licensing. (market=NO)
  char field_options[] = "?market=NO&fields=name%2C+description%2C+tracks%28total%29";

  int url_length = strlen(url_api_dest) + strlen(field_options) + strlen(playlistID);
  requestURL = malloc(url_length);
  requestURL[0] = '\0';
  strcat(requestURL, url_api_dest);
  strcat(requestURL, playlistID);
  strcat(requestURL, field_options);

  return curl_request(authorization, requestURL); 
}



char* getPlaylistContentSpotify(char* playlistID, SpotifyAccess* ad, int offset){

  char* requestURL; // free used in curl_request func
  char* authorization; // Same as above.
  int g_type_len = (int)strlen(ad->type); 


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
  char field_options[] = "/tracks?market=NO&fields=items%28track%28name%2C+external_urls%2C+preview_url%2C+duration_ms%2C+album%28name%29%2C+artists%28name%29%29&limit=10&offset=";
  //TODO: Set offset to 100.
  //Finds the number of digits in the offset number.
  int offset_length = snprintf( NULL, 0, "%d", offset)+1; //+1 cause space for \0
  char* offset_str = malloc( offset_length );
  snprintf( offset_str, offset_length + 1, "%d", offset);

  int url_length = strlen(url_api_dest) + strlen(field_options) + strlen(playlistID) + offset_length;
  requestURL = calloc(url_length, sizeof(char));

  strcat(requestURL, url_api_dest);
  strcat(requestURL, playlistID);
  strcat(requestURL, field_options);
  strcat(requestURL, offset_str);

  free(offset_str);
  offset_str = NULL;

 
  return curl_request(authorization, requestURL);
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


void spotify_access_delete(SpotifyAccess **access_obj){
  
  if(access_obj ==  NULL) return;
  memset((*access_obj)->token, '\0', strlen((*access_obj)->token));
  free((*access_obj)->token);
  free((*access_obj)->type);
  free((*access_obj)->time_recieved);
  free((*access_obj));
}

SpotifyPlaylist* parce_spotify_playlist_data(const char* data)
{ 
  
  SpotifyPlaylist* new_playlist;
  new_playlist = spotify_playlist_init();
  if (new_playlist == NULL)
  {
    fprintf(stderr, "Error occured when initializing spotify playlist.\n");
    return NULL;
  }
  
  cJSON *json = cJSON_Parse(data);
  if (json == NULL)
  {
    fprintf(stderr, "Error parsing JSON.\n");
    goto cleanup;
  }

  cJSON *name = cJSON_GetObjectItem(json, "name");
  if (name == NULL)
  {
    fprintf(stderr, "Error fetching 'name' from tracks object string.\n");
    goto cleanup;
  }
  
  cJSON *description = cJSON_GetObjectItem(json, "description");
  if (description == NULL)
  {
    fprintf(stderr, "Error fetching 'description' from tracks object string.\n");
    goto cleanup;
  }

  cJSON *tracks = cJSON_GetObjectItem(json, "tracks");
  if (tracks == NULL)
  {
    fprintf(stderr, "Error fetching 'description' from tracks object string.\n");
    goto cleanup;
  }
  
  cJSON *total_tracks = cJSON_GetObjectItem(tracks, "total");
  if (total_tracks == NULL)
  {
    fprintf(stderr, "Error fetching 'description' from tracks object string.\n");
    goto cleanup;
  }
  
  new_playlist->name = malloc(strlen(name->valuestring));
  if(new_playlist->name == NULL)
  {
    fprintf(stderr, "Error allocating memory for spotify playlist name\n");
    goto cleanup;
  }

  new_playlist->name[0] = '\0';
  strcpy(new_playlist->name, name->valuestring);

  if (description->valuestring == NULL)
  {
    new_playlist->description = malloc(1);
    if (new_playlist->description == NULL)
    {
      fprintf(stderr, "Error allocating memory for spotify playlist description.\n");
      goto cleanup;
    }
    
    new_playlist->description[0] = '\0';
  }
  else
  {
    new_playlist->description = malloc(strlen(description->valuestring));
    if (new_playlist->description == NULL)
    {
      fprintf(stderr, "Error allocating memory for spotify playlist description.\n");
      goto cleanup;
    }
    strcpy(new_playlist->description, description->valuestring);
  }

  new_playlist->total_tracks = total_tracks->valueint;

  cJSON_Delete(json);
  return new_playlist;

  cleanup:
  cJSON_Delete(json);
  free(new_playlist->name);
  free(new_playlist->description);

  free(new_playlist);
  return NULL;
}


int parce_spotify_track_data(char* data, ListSpotifyTracks** list_head)
{ 

  cJSON *json = cJSON_Parse(data);
  if (json == NULL)
  {
    fprintf(stderr, "Error parsing JSON.\n");
    goto cleanup;
  }

  cJSON *items = cJSON_GetObjectItem(json, "items");
  if (items == NULL)
  {
    fprintf(stderr, "Error fetching 'items' object from tracks object string.");
    goto cleanup;
  }

  int number_of_songs = cJSON_GetArraySize(items);
  
  cJSON *item = NULL;
  cJSON_ArrayForEach(item, items)
  {
    cJSON *track = cJSON_GetObjectItem(item, "track");
    if (track==NULL) continue;
    
    cJSON *track_name = cJSON_GetObjectItem(track, "name");
    cJSON *album = cJSON_GetObjectItem(track, "album");
    cJSON *album_name = cJSON_GetObjectItem(album, "name");
    //After artists
    cJSON *duration_ms = cJSON_GetObjectItem(track, "duration_ms");
    cJSON *preview_url = cJSON_GetObjectItem(track, "preview_url");
    cJSON *external_urls = cJSON_GetObjectItem(track, "external_urls");
    cJSON *spotify_url = cJSON_GetObjectItem(external_urls, "spotify");

    cJSON *artists = cJSON_GetObjectItem(track, "artists");
    char* artists_str = NULL;
    int arr_size = cJSON_GetArraySize(artists);
    cJSON *artist = NULL;
    int i = 0;
    cJSON_ArrayForEach(artist, artists){
      if(artist == NULL)
      {
        fprintf(stderr, "Error getting artist");
        i++;
        continue;
      }

      cJSON *artist_name = cJSON_GetObjectItem(artist, "name");
      if(artist_name == NULL)
      {
        fprintf(stderr, "Error getting artist name");
        i++;
        continue;
      }
      
      if(arr_size == 1){
        artists_str = calloc(strlen(artist_name->valuestring) + 1, sizeof(char));
        if(artists_str == NULL){
          fprintf(stderr, "Error allocating Artists.");
          return -1;
        }
        strcpy(artists_str, artist_name->valuestring);
      }
      else
      {
        if (i == 0)
        {
          artists_str = calloc(strlen(artist_name->valuestring) + 1, sizeof(char));
          if(artists_str == NULL){
            fprintf(stderr, "Error allocating Artists.");
            return -1;
          }
          strcpy(artists_str, artist_name->valuestring);
        }
        else{
          size_t artist_str_size = strlen(artists_str) + strlen(artist_name->valuestring) + strlen(", ") + 1;
          artists_str = realloc(artists_str, artist_str_size); 
          if (artists_str == NULL){
            fprintf(stderr, "Error reallocating memory for artists.");
            return -1;
          }
          strcat(artists_str, ", ");
          strcat(artists_str, artist_name->valuestring);
        }
      }
      i++;
    }
    if (!append_spotify_track(
        list_head,
        track_name->valuestring, 
        album_name->valuestring, 
        artists_str, 
        duration_ms->valueint, 
        preview_url->valuestring, 
        spotify_url->valuestring)){
          printf("Failed to add spotify track.");
        }  
    free(artists_str);
    artists_str = NULL;
  }

  cleanup:
  cJSON_Delete(json);

  return number_of_songs;
}


SpotifyPlaylist* get_spotify_playlist(SpotifyAccess *access, char *playlist_id ){
  SpotifyPlaylist* playlist;

  int songs_recieved = 0;
  int total = 0;
  char* playlist_info = getPlaylistInfoSpotify(playlist_id, access);
  if (playlist_info == NULL)
  {
    fprintf(stderr, "No playlist info receaved from spotify.\n");
    goto cleanup;
  }
  
  playlist = parce_spotify_playlist_data(playlist_info);
  if (playlist == NULL)
  {
    fprintf(stderr, "Error occurred when parcing playlist data.\n");
    goto cleanup;
  }
  
  free(playlist_info);
  playlist_info = NULL;

  printf("Fetching playlist:\n");
  printf("\tName: %s\n", playlist->name);
  printf("\tTotal songs: %d\n", playlist->total_tracks);
  printf("\tDescription: %s\n", playlist->description);

  do{
    char* content = getPlaylistContentSpotify(playlist_id, access, total);
    if (content == NULL)
    {
      fprintf(stderr, "No tracks receved from spotify.\n");
      goto cleanup;
    }
    
    songs_recieved = parce_spotify_track_data(content, &playlist->track_list);
    total += songs_recieved;
    printf("\rReceved and parced %d of %d songs.", total, playlist->total_tracks);
    free(content);
    content = NULL;
    fflush(stdout);
    }
    while (songs_recieved == 100);
  printf("\nFinished fetching data from playlist '%s'.\n", playlist->name);
  return playlist;

  cleanup:

  free(playlist);
  playlist = NULL;
  free(playlist_info);
  playlist_info = NULL;
  return NULL;
}