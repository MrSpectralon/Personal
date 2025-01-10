#include "../header_files/spotify_methods.h"
#include "../header_files/curl_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Requests an authentication token from spotify.
 * Returns a string containing the JSON
 */
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

/**
 * fetches information about a specified spotify playlist.
 * Needs a valid playlist ID and valid access token before use.
 */
char* get_playlist_info_spotify (OauthAccess *access_data)
{

    //Preparing approperiate URL for fetching playlist data. 
    char url_api_dest[] = "https://api.spotify.com/v1/playlists/";
    // At the moment hardcoded for Norwegian music licensing. (market=NO)
    char field_options[] = "?market=NO&fields=name%2C+description%2C+tracks%28total%29";

    char *requestURL; 
    char *authorization; 
    int g_type_len = (int) strlen (access_data->type);

    //Preparing authentification
    char auth[] = "Authorization: Bearer ";

    int auth_length = strlen (auth) + g_type_len + strlen (access_data->token) + 2; 
    authorization = malloc (auth_length);
    authorization[0] = '\0';
    strcat (authorization, auth);
    strcat (authorization, access_data->token);

    int url_length = strlen (url_api_dest) + strlen (field_options) + strlen (access_data->playlist);
    requestURL = malloc (url_length + 1);
    requestURL[0] = '\0';
    strcat (requestURL, url_api_dest);
    strcat (requestURL, access_data->playlist);
    strcat (requestURL, field_options);

    char* reply = curl_get_request (authorization, requestURL);
    free(authorization);
    free(requestURL);
    return reply;
}


/**
 * Fetches up to 100 songs from a playlist.
 * The offset parameter is used when the playlist contains more than 100 songs.
 * Offset should start at 0 and iterate by 100.
 */
char* get_playlist_content_spotify (OauthAccess *ad, int offset)
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
	    + strlen (ad->playlist) 
	    + offset_length;

    requestURL = calloc (url_length, sizeof (char));

    strcat (requestURL, url_api_dest);
    strcat (requestURL, ad->playlist);
    strcat (requestURL, field_options);
    strcat (requestURL, offset_str);

    free (offset_str);
    offset_str = NULL;
    char* request = curl_get_request(authorization, requestURL);
    free(authorization);
    free(requestURL);
    return request;

}

/**
 * Parces the JSON reply from spotify, adding all track information into a linked list of tracks.
 * 
 */
Playlist* parce_spotify_playlist_data (const char *data)
{
    Playlist* new_playlist = playlist_init();
    if (new_playlist == NULL)
    {
	fprintf (stderr, "Error occured when initializing spotify playlist.\n");
	return NULL;
    }
    new_playlist->service = SPOTIFY;

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
	fprintf (stderr, "Error fetching 'description' from tracks object string.\n");
	goto cleanup;
    }

    new_playlist->name = malloc (strlen (name->valuestring) + 1);
    if (new_playlist->name == NULL)
    {
	fprintf (stderr, "Error allocating memory for spotify playlist name\n");
	goto cleanup;
    }

    strcpy (new_playlist->name, name->valuestring);

    if (description->valuestring == NULL)
    {
	new_playlist->description = malloc (1);
	if (new_playlist->description == NULL)
	{
	    fprintf (stderr, "Error allocating memory for spotify playlist description.\n");
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

/**
 * Sub module of a bigger function.
 * As the name implies - Parces json and adds the findings into a linked list of tracks.
 * Returns the number of songs parced.
 */
int parce_spotify_track_data (char *data, TrackList** list_head)
{
    int number_of_songs = 0;
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

    cJSON *item = NULL;
    cJSON_ArrayForEach (item, items)
    {
	cJSON *track = cJSON_GetObjectItem (item, "track");
	if (track == NULL) continue;

	number_of_songs++;	
	handle_json_track_data(track, list_head);
    }

  cleanup:
    cJSON_Delete (json);

    return number_of_songs;
}


/**
 * Sub module of a bigger function.
 * Looks through JSON objects for spotify track data.
 * Adds matching data into a list of tracks.
 */
void handle_json_track_data(cJSON* track_obj, TrackList** list_head)
{
    cJSON *track_name = cJSON_GetObjectItem (track_obj, "name");
    cJSON *album = cJSON_GetObjectItem (track_obj, "album");
    cJSON *album_name = cJSON_GetObjectItem (album, "name");
    cJSON *duration_ms = cJSON_GetObjectItem (track_obj, "duration_ms");
    // cJSON *preview_url = cJSON_GetObjectItem (track_obj, "preview_url");
    cJSON *external_urls = cJSON_GetObjectItem (track_obj, "external_urls");
    cJSON *spotify_url = cJSON_GetObjectItem (external_urls, "spotify");

    cJSON *artists = cJSON_GetObjectItem (track_obj, "artists");
    
    char *artists_str = handle_artists_from_json(artists);
    
    Track* new = track_init();
    if (new == NULL) {
	return;
    }
    new->name = strdup(track_name->valuestring);
    if (new->name == NULL) goto cleanup;

    new->album = strdup(album_name->valuestring);
    if (new->album == NULL) goto cleanup;
    
    new->artist = strdup(artists_str);
    if (new->artist == NULL) goto cleanup;
    
    track_set_duration(&new, SPOTIFY, (void*)&duration_ms->valueint);
    if (new->duration == NULL) goto cleanup;
    
    new->url = strdup(spotify_url->valuestring);
    if (new->url == NULL) goto cleanup;
    playlist_append(new, list_head);

    return;

cleanup:
    track_free(&new);
    fprintf(stderr, "Failed to add spotify track to list.\n");
}


/**
 * Sub module of a larger funtion.
 * Finds all artists participating in a song and returns the findings in a string.
 */
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
	    fprintf (stderr, "Error reallocating memory for artists.\n");
	    return NULL;
	}
	strcat (artists_str, ", ");
	strcat (artists_str, artist_name->valuestring);
	i++;
    }
    return artists_str;
}


/**
 * Fetches a spotify playlist by ID.
 */
Playlist* get_spotify_playlist (OauthAccess *access)
{
    Playlist* playlist;

    int songs_recieved = 0;
    int total = 0;
    char *playlist_info = get_playlist_info_spotify(access);
    if (playlist_info == NULL)
    {
	fprintf (stderr, "No playlist info receaved from spotify.\n");
	goto cleanup;
    }

    playlist = parce_spotify_playlist_data(playlist_info);
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
	char *content = get_playlist_content_spotify (access, total);
	if (content == NULL)
	{
	    fprintf (stderr, "No tracks receved from spotify.\n");
	    goto cleanup;
	}

	songs_recieved =
	    parce_spotify_track_data(content, &playlist->tracks);
	total += songs_recieved;
	printf ("\rReceved and parced %d of %d songs.", total,
	    playlist->total_tracks);
	free (content);
	content = NULL;
	fflush (stdout);
    }
    while (songs_recieved != playlist->total_tracks);
    
    printf ("\nFinished fetching data from playlist '%s'.\n", playlist->name);
    return playlist;

  cleanup:
    free (playlist);
    playlist = NULL;
    free (playlist_info);
    playlist_info = NULL;
    return NULL;
}
