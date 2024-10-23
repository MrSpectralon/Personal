#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev
#include <cjson/cJSON.h>
#include "./spotify_track_list.h"
#include "./helper_methods.h"
#include "./curl_handler.h"
#include "./oauth_handler.h"



SpotifyPlaylist* get_spotify_playlist(OauthAccess *access, char *playlist_id);

char* get_playlist_content_spotify(char* playlistID, OauthAccess* ad, int offset);
int parce_spotify_track_data(char* data, ListSpotifyTracks** list_head);
SpotifyPlaylist* parce_spotify_playlist_data(const char* data);

void handle_json_track_data(cJSON* track_obj, ListSpotifyTracks** list_head);
char* handle_artists_from_json(cJSON* artists_obj);

char* get_auth_token_spotify(const char* clientID, const char* clientSecret);
