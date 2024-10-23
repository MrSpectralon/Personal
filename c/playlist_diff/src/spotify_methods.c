#include "../header_files/spotify_methods.h"


char* get_auth_token_spotify(const char *clientID, const char *clientSecret)
{
    //Should be marked as volatile so memset actually executes.
    char *curlPostField = NULL;

    const char grantTypeAndClientID[] = "grant_type=client_credentials&client_id=";
    const char cliS[] = "&client_secret=";
    const char content_type[] = "Content-Type: application/x-www-form-urlencoded";
    const char destination_url[] = "https://accounts.spotify.com/api/token";

    long fieldSize = 
        strlen (grantTypeAndClientID)
        + strlen (clientID) 
        + strlen (cliS) 
        + strlen (clientSecret);

    curlPostField = calloc (fieldSize + 1, sizeof (char));
    if (curlPostField == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for curl post field.\n");
        return NULL;
    }
    strcat (curlPostField, grantTypeAndClientID);
    strcat (curlPostField, clientID);
    strcat (curlPostField, cliS);
    strcat (curlPostField, clientSecret);

    char* response = curl_post_request(destination_url, 
                content_type, curlPostField, fieldSize);
    
    memset(curlPostField, 0, fieldSize);
    free (curlPostField);
    curlPostField = NULL;
    return response;
}


char* get_playlist_info_spotify (char *playlistID, OauthAccess *access_data)
{
    //Preparing approperiate URL for fetching playlist data. 
    char url_api_dest[] = "https://api.spotify.com/v1/playlists/";
    // At the moment hardcoded for Norwegian music licensing. (market=NO)
    char field_options[] =
	"?market=NO&fields=name%2C+description%2C+tracks%28total%29";

    char *requestURL; 
    char *authorization; 
    int g_type_len = (int) strlen (access_data->type);

    //Preparing authentification
    char auth[] = "Authorization: ";
    char type[g_type_len + 2];
    type[0] = '\0';
    strcat (type, access_data->type);
    type[g_type_len] = ' ';
    type[g_type_len + 1] = '\0';

    int auth_length = strlen (auth) + g_type_len + strlen (access_data->token) + 2; 
    authorization = malloc (auth_length);
	authorization[0] = '\0';
    strcat (authorization, auth);
    strcat (authorization, type);
    strcat (authorization, access_data->token);

    int url_length = strlen (url_api_dest) + strlen (field_options) + strlen (playlistID);
    requestURL = malloc (url_length + 1);
    requestURL[0] = '\0';
    strcat (requestURL, url_api_dest);
    strcat (requestURL, playlistID);
    strcat (requestURL, field_options);
	char* reply = curl_get_request (authorization, requestURL);
	free(authorization);
	free(requestURL);
	return reply;   
}



char* get_playlist_content_spotify (char *playlistID, OauthAccess *ad, int offset)
{

    //Preparing approperiate URL for fetching playlist data. 
    char url_api_dest[] = "https://api.spotify.com/v1/playlists/";

    // At the moment hardcoded for Norwegian music licensing. (market=NO)
    char field_options[] =
	"/tracks?market=NO&fields=items%28track%28name%2C+external_urls%2C+preview_url%2C+duration_ms%2C+album%28name%29%2C+artists%28name%29%29&limit=100&offset=";

    char *requestURL;		// free used in curl_request func
    char *authorization;	// Same as above.
    int g_type_len = (int) strlen (ad->type);


    //Preparing authentification
    char auth[] = "Authorization: ";
    char type[g_type_len + 2];
    type[0] = '\0';
    strcat (type, ad->type);
    type[g_type_len] = ' ';
    type[g_type_len + 1] = '\0';

    
    int auth_length = strlen (auth) + g_type_len + 1 + strlen (ad->token); // +1 because g_type is 1 short.
    authorization = calloc (auth_length + 1, sizeof (char));
    strcat (authorization, auth);
    strcat (authorization, type);
    strcat (authorization, ad->token);


	//Finds the number of digits in the offset number.
    int offset_length = snprintf (NULL, 0, "%d", offset) + 1;	//+1 cause space for \0
    char *offset_str = malloc (offset_length);
    snprintf (offset_str, offset_length + 1, "%d", offset);

    int url_length =
			strlen (url_api_dest) 
			+ strlen (field_options) 
			+ strlen (playlistID) 
			+ offset_length;

    requestURL = calloc (url_length, sizeof (char));

    strcat (requestURL, url_api_dest);
    strcat (requestURL, playlistID);
    strcat (requestURL, field_options);
    strcat (requestURL, offset_str);

    free (offset_str);
    offset_str = NULL;
	char* request = curl_get_request(authorization, requestURL);
	free(authorization);
	free(requestURL);
	return request;

}


SpotifyPlaylist* parce_spotify_playlist_data (const char *data)
{
    SpotifyPlaylist* new_playlist;
    new_playlist = spotify_playlist_init ();
    if (new_playlist == NULL)
	{
		fprintf (stderr, "Error occured when initializing spotify playlist.\n");
		return NULL;
	}

    cJSON *json = cJSON_Parse (data);
    if (json == NULL)
	{
		fprintf (stderr, "Error parsing JSON.\n");
		goto cleanup;
	}

    cJSON *name = cJSON_GetObjectItem (json, "name");
    if (name == NULL)
	{
		fprintf (stderr, "Error fetching 'name' from tracks object string.\n");
		goto cleanup;
	}

    cJSON *description = cJSON_GetObjectItem (json, "description");
    if (description == NULL)
	{
		fprintf (stderr, "Error fetching 'description' from tracks object string.\n");
		goto cleanup;
	}

    cJSON *tracks = cJSON_GetObjectItem (json, "tracks");
    if (tracks == NULL)
	{
		fprintf (stderr, "Error fetching 'description' from tracks object string.\n");
		goto cleanup;
	}

    cJSON *total_tracks = cJSON_GetObjectItem (tracks, "total");
    if (total_tracks == NULL)
	{
		fprintf (stderr,
			"Error fetching 'description' from tracks object string.\n");
		goto cleanup;
	}

    new_playlist->name = malloc (strlen (name->valuestring) + 1);
    if (new_playlist->name == NULL)
	{
		fprintf (stderr,
			"Error allocating memory for spotify playlist name\n");
		goto cleanup;
	}

    strcpy (new_playlist->name, name->valuestring);

    if (description->valuestring == NULL)
	{
		new_playlist->description = malloc (1);
		if (new_playlist->description == NULL)
		{
			fprintf (stderr,
					"Error allocating memory for spotify playlist description.\n");
			goto cleanup;
		}
		new_playlist->description[0] = '\0';
	}
    else
	{
		new_playlist->description = malloc (strlen(description->valuestring) + 1);
		if (new_playlist->description == NULL)
		{
			fprintf (stderr, "Error allocating memory for spotify playlist description.\n");
			goto cleanup;
		}
		strcpy (new_playlist->description, description->valuestring);
	}

    new_playlist->total_tracks = total_tracks->valueint;

    cJSON_Delete (json);
    return new_playlist;

  	cleanup:
    cJSON_Delete (json);
    free (new_playlist->name);
    free (new_playlist->description);
    free (new_playlist);
    return NULL;
}


int parce_spotify_track_data (char *data, ListSpotifyTracks **list_head)
{
    cJSON *json = cJSON_Parse (data);
    if (json == NULL)
	{
		fprintf (stderr, "Error parsing JSON.\n");
		goto cleanup;
	}

    cJSON *items = cJSON_GetObjectItem (json, "items");
    if (items == NULL)
	{
		fprintf (stderr, "Error fetching 'items' object from tracks object string.\n");
		goto cleanup;
	}

    int number_of_songs = cJSON_GetArraySize (items);

    cJSON *item = NULL;
    cJSON_ArrayForEach (item, items)
    {
		cJSON *track = cJSON_GetObjectItem (item, "track");
		if (track == NULL) continue;
		
		handle_json_track_data(track, list_head);
    }

  	cleanup:
    cJSON_Delete (json);

    return number_of_songs;
}

void handle_json_track_data(cJSON* track_obj, ListSpotifyTracks** list_head)
{
	cJSON *track_name = cJSON_GetObjectItem (track_obj, "name");
	cJSON *album = cJSON_GetObjectItem (track_obj, "album");
	cJSON *album_name = cJSON_GetObjectItem (album, "name");
	cJSON *duration_ms = cJSON_GetObjectItem (track_obj, "duration_ms");
	cJSON *preview_url = cJSON_GetObjectItem (track_obj, "preview_url");
	cJSON *external_urls = cJSON_GetObjectItem (track_obj, "external_urls");
	cJSON *spotify_url = cJSON_GetObjectItem (external_urls, "spotify");

	cJSON *artists = cJSON_GetObjectItem (track_obj, "artists");
	
	char *artists_str = handle_artists_from_json(artists);
	
	if (!append_spotify_track (list_head,
				track_name->valuestring,
				album_name->valuestring,
				artists_str,
				duration_ms->valueint,
				preview_url->valuestring,
				spotify_url->valuestring))
	{
		printf ("Failed to add spotify track.\n");
	}
	free (artists_str);
}

char* handle_artists_from_json(cJSON* artists_obj)
{
	int arr_size = cJSON_GetArraySize (artists_obj);
	cJSON *artist = NULL;
	char* artists_str;
	int i = 0;
	cJSON_ArrayForEach (artist, artists_obj)
	{
		if (artist == NULL)
		{
			fprintf (stderr, "Error getting artist\n");
			i++;
			continue;
		}
		cJSON *artist_name = cJSON_GetObjectItem (artist, "name");
		if (artist_name == NULL)
		{
			fprintf (stderr, "Error getting artist name\n");
			return NULL;
		}

		if (arr_size == 1)
		{
			artists_str =
			calloc (strlen (artist_name->valuestring) + 1, sizeof (char));
			
			if (artists_str == NULL)
			{
				fprintf (stderr, "Error allocating Artists.\n");
				return NULL;
			}
			strcpy (artists_str, artist_name->valuestring);
			return artists_str;
		}
		
		//Initializing string for first iteration
		if (i == 0)
		{
			artists_str =
			calloc (strlen (artist_name->valuestring) + 1, sizeof (char));
			if (artists_str == NULL)
			{
				fprintf (stderr, "Error allocating Artists.\n");
				return NULL;
			}
			strcpy (artists_str, artist_name->valuestring);
			i++;
			continue;
		}

		size_t artist_str_size =
			strlen (artists_str) +
			strlen (artist_name->valuestring) +
			strlen (", ") + 1;
		artists_str = realloc (artists_str, artist_str_size);
		if (artists_str == NULL)
		{
			fprintf (stderr,
				"Error reallocating memory for artists.\n");
			return NULL;
		}
		strcat (artists_str, ", ");
		strcat (artists_str, artist_name->valuestring);
		i++;
	}
	return artists_str;
}


SpotifyPlaylist* get_spotify_playlist (OauthAccess *access, char *playlist_id)
{
    SpotifyPlaylist *playlist;

    int songs_recieved = 0;
    int total = 0;
    char *playlist_info = get_playlist_info_spotify (playlist_id, access);
    if (playlist_info == NULL)
	{
		fprintf (stderr, "No playlist info receaved from spotify.\n");
		goto cleanup;
	}

    playlist = parce_spotify_playlist_data (playlist_info);
    if (playlist == NULL)
	{
		fprintf (stderr, "Error occurred when parcing playlist data.\n");
		goto cleanup;
	}

    free (playlist_info);
    playlist_info = NULL;

    printf ("Fetching playlist:\n");
    printf ("\tName: %s\n", playlist->name);
    printf ("\tTotal songs: %d\n", playlist->total_tracks);
    printf ("\tDescription: %s\n", playlist->description);

    do
	{
		char *content = get_playlist_content_spotify (playlist_id, access, total);
		if (content == NULL)
		{
			fprintf (stderr, "No tracks receved from spotify.\n");
			goto cleanup;
		}

		songs_recieved =
			parce_spotify_track_data (content, &playlist->track_list);
		total += songs_recieved;
		printf ("\rReceved and parced %d of %d songs.", total,
			playlist->total_tracks);
		free (content);
		content = NULL;
		fflush (stdout);
	}
    while (songs_recieved != 100);
    
	printf ("\nFinished fetching data from playlist '%s'.\n", playlist->name);
    return playlist;

  cleanup:
    free (playlist);
    playlist = NULL;
    free (playlist_info);
    playlist_info = NULL;
    return NULL;
}