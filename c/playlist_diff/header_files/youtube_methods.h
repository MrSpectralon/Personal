#pragma once
#include "oauth_handler.h"
#include "playlist.h"


char* create_JWT(const char* service_account, const char* user_email, const char* service_acc_private_key, const size_t key_len);
char* get_auth_token_youtube(const char* service_account, const char* service_account_secret, const size_t secret_len);

Playlist* get_youtube_playlist(OauthAccess* access);

char* get_youtube_playlist_details(OauthAccess* access);
int parce_youtube_playlist_details(Playlist** playlist, char* data);

char* get_youtube_playlist_tracks(OauthAccess* access, char* page_token);
int parce_youtube_playlist_tracks(TrackList** track_list, char* track_data, char** next_page);


