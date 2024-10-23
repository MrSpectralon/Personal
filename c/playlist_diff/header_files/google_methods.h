#pragma once

#include "helper_methods.h"
#include "cjson/cJSON.h"

cJSON* create_JWT();

char* get_auth_token_google (const char *clientID, const char *clientSecret);