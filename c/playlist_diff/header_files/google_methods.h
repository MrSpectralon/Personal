#pragma once
#include "oauth_handler.h"
#include "helper_methods.h"
#include "curl_handler.h"
#include "cjson/cJSON.h"
#include "youtube_playlist.h"


char* create_JWT(const char* service_account, const char* user_email, const char* service_acc_private_key, const size_t key_len);
char* get_auth_token_youtube(const char* service_account, const char* service_account_secret, const size_t secret_len);

YoutubePlaylist* get_youtube_playlist(OauthAccess* access, char* playlist_id);

char* get_youtube_playlist_details(char* playlist_id, OauthAccess* access);
int parce_youtube_playlist_details(YoutubePlaylist** playlist, char* data);

char* get_youtube_playlist_tracks(char* playlist_id, OauthAccess* access, char* page_token);
int parce_youtube_playlist_tracks(YoutubeTrackList** track_list, char* track_data, char** next_page);


