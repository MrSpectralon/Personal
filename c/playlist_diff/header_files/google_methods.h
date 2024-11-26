#pragma once
#include "oauth_handler.h"
#include "helper_methods.h"
#include "curl_handler.h"
#include "cjson/cJSON.h"


char* create_JWT(const char* service_account, const char* user_email, const char* service_acc_private_key, const size_t key_len);
char* get_auth_token_google (const char* service_account, const char* service_account_secret, const size_t secret_len);
