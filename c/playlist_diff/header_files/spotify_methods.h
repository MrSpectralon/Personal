#pragma once

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev
#include <cjson/cJSON.h>
#include "./spotify_track_list.h"
#include "./helper_methods.h"


typedef struct {
	char *token;
	char *type;
	int duration;
	struct tm *time_recieved;
    
} SpotifyAccess;


SpotifyPlaylist* get_spotify_playlist(SpotifyAccess *access, char *playlist_id);

char* getPlaylistContentSpotify(char* playlistID, SpotifyAccess* ad, int offset);
int parce_spotify_track_data(char* data, ListSpotifyTracks** list_head);
SpotifyPlaylist* parce_spotify_playlist_data(const char* data);


size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);

char* getAuthTokenSpotify(char* clientID, char* clientSecret);


void getAuthKey(CURL *opts, CURLcode res);

SpotifyAccess *spotify_access_init(char* auth_reply);
void print_spotify_access(SpotifyAccess *access_obj); 
void spotify_access_delete(SpotifyAccess** access_obj);