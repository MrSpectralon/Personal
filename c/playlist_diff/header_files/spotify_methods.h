#pragma once

#include <curl/curl.h> 
#include <cjson/cJSON.h>
#include "./playlist.h"
#include "./oauth_handler.h"



Playlist* get_spotify_playlist(OauthAccess *access);

char* get_playlist_content_spotify(OauthAccess* ad, int offset);
int parce_spotify_track_data(char* data, TrackList** list_head);
Playlist* parce_spotify_playlist_data(const char* data);

void handle_json_track_data(cJSON* track_obj, TrackList** list_head);
char* handle_artists_from_json(cJSON* artists_obj);

char* get_auth_token_spotify(const char* clientID, const char* clientSecret);
