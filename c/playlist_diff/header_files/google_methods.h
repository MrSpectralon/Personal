#pragma once

#include "helper_methods.h"
#include "cjson/cJSON.h"

char* create_JWT(const char* clientID, const char* clientSecret);

char* get_auth_token_google (const char *clientID, const char *clientSecret);
