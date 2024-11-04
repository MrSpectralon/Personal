#include "../header_files/google_methods.h"
#include <cjson/cJSON.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>



char* create_JWT(const char* clientID, const char* clientSecret)
{
    char* jwt_h_str = NULL;
    cJSON* jwt_header = cJSON_CreateObject();
    char* header_encode = NULL;

    cJSON_AddStringToObject(jwt_header, "alg", "HS256");
    cJSON_AddStringToObject(jwt_header, "typ", "JWT");
    jwt_h_str = cJSON_PrintUnformatted(jwt_header);
    cJSON_Delete(jwt_header);
    jwt_header = NULL;
    size_t h_len = strlen(jwt_h_str);
    
    header_encode = base64url_encode(jwt_h_str, h_len);
    printf("Json: %s\n", jwt_h_str);
    printf("Base: %s\n", header_encode);
    
    //######### Making the JWT paylaod ############
    //{
    //
    //   "iss": "your-service-account-email@project-id.iam.gserviceaccount.com",
    //   "scope": https://www.googleapis.com/auth/youtube",
    //   "aud": "https://oauth2.googleapis.com/token",
    //   "exp": 1623443462,
    //   "iat": 1623441662
    // }

    cJSON* jwt_payload = cJSON_CreateObject();

    cJSON_AddStringToObject(jwt_payload, "iss", "service@playlist-diff-435315.iam.gserviceaccount.com");
    cJSON_AddStringToObject(jwt_payload, "scope", "https://www.googleapis.com/auth/youtube");
    cJSON_AddStringToObject(jwt_payload, "aud", "https://oauth2.googleapis.com/token");


  cleanup:
    free(jwt_h_str);
    jwt_h_str = NULL;
    free(header_encode);
    header_encode = NULL;
    return NULL;
    

}


char* get_auth_token_google (const char *clientID, const char *clientSecret)
{
    printf("Client ID = %s\n", clientID);
    printf("Client Secret = %s\n", clientSecret);
    create_JWT(clientID, clientSecret);
    return NULL;
}

