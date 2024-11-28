#pragma once
#include "oauth_handler.h"
#include "helper_methods.h"
#include "curl_handler.h"
#include "cjson/cJSON.h"
#include "youtube_playlist.h"


char* create_JWT(const char* service_account, const char* user_email, const char* service_acc_private_key, const size_t key_len);
char* get_auth_token_youtube(const char* service_account, const char* service_account_secret, const size_t secret_len);

YoutubePlaylist* get_youtube_playlist(OauthAccess* access, char* playlist_id);

