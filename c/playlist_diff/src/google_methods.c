#include "../header_files/google_methods.h"
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>



char* create_JWT(const char* service_account, const char* service_acc_private_key, const size_t key_len)
{
    char* jwt_p_str = NULL;
    char* jwt_h_str = NULL;
    char* payload_b64 = NULL;
    char* signature_str = NULL;
    unsigned char* signature = NULL;
    char signature_hex[65] = {};
    char* jwt = NULL;

    cJSON* jwt_header = cJSON_CreateObject();
    cJSON* jwt_payload = cJSON_CreateObject();
    char* header_b64 = NULL;
    //########## Making JWT header ##############
    // {
    //   "alg": "HS256",
    //   "typ": "JWT"
    // }
    cJSON_AddStringToObject(jwt_header, "alg", "HS256");
    cJSON_AddStringToObject(jwt_header, "typ", "JWT");
    jwt_h_str = cJSON_PrintUnformatted(jwt_header);
    if (jwt_h_str == NULL) {
        goto cleanup;
    }
    size_t h_len = strlen(jwt_h_str);
    
    header_b64 = base64url_encode(jwt_h_str, h_len);
    if (header_b64 == NULL) {
        goto cleanup;
    }
    printf("Json: %s\n", jwt_h_str);
    printf("Base: %s\n", header_b64);
    cJSON_Delete(jwt_header);
    free(jwt_h_str);
    jwt_header = NULL;
    jwt_h_str = NULL;

    //######### Making the JWT paylaod ############
    //{
    //   "iss": "your-service-account-email@project-id.iam.gserviceaccount.com",
    //   "scope": https://www.googleapis.com/auth/youtube",
    //   "aud": "https://oauth2.googleapis.com/token",
    //   "exp": <integer>unix time + iat,
    //   "iat": unix time
    // }

    time_t iat = time(NULL);
    time_t exp = iat + 3600;
    

    cJSON_AddStringToObject(jwt_payload, "iss", service_account);
    cJSON_AddStringToObject(jwt_payload, "scope", "https://www.googleapis.com/auth/youtube");
    cJSON_AddStringToObject(jwt_payload, "aud", "https://oauth2.googleapis.com/token");
    cJSON_AddNumberToObject(jwt_payload, "iat", iat);
    cJSON_AddNumberToObject(jwt_payload, "exp", exp);

    jwt_p_str = cJSON_PrintUnformatted(jwt_payload);
    payload_b64 = base64url_encode(jwt_p_str, strlen(jwt_p_str));
    if (payload_b64 == NULL) {
        goto cleanup;
    }
    cJSON_Delete(jwt_payload);
    free(jwt_p_str);
    jwt_payload = NULL;
    jwt_p_str = NULL;

    size_t msg_len = strlen(payload_b64) + strlen(header_b64);
    signature_str = malloc(msg_len + 1);
    if (signature_str == NULL) {
        goto cleanup;
    }
    signature_str[0] = '\0';
    
    strcpy(signature_str, header_b64);
    strcat(signature_str, payload_b64);

    signature = hmac_sha256(service_acc_private_key, key_len, signature_str, msg_len);
    free(signature_str);
    if (signature == NULL) {
        goto cleanup;
    }
    for (size_t i = 0; i < 32; i++) {
        sprintf(signature_hex+(2*i), "%02x", signature[i]);
    }
    free(signature);
    signature = NULL;

    //67 because of two '.' for separation, 64 for the 32byte signature + 1 for null termination.
    jwt = malloc(msg_len + 67);
    if (jwt == NULL) {
        goto cleanup;
    }
    jwt[0] = '\0';
    strcpy(jwt, header_b64);
    strcat(jwt, ".");
    strcat(jwt, payload_b64);
    strcat(jwt, ".");
    strcat(jwt, signature_hex);
    
    free(header_b64);
    free(payload_b64);

    return jwt;

  cleanup:
    cJSON_Delete(jwt_header);
    cJSON_Delete(jwt_payload);
    free(signature_str);
    free(signature);
    free(jwt_h_str);
    free(header_b64);
    free(jwt_p_str);
    free(payload_b64);

    return NULL;
    

}


char* get_auth_token_google (const char* service_account, const char* service_account_secret, const size_t secret_len)
{
    char* jwt = NULL;
    jwt = create_JWT(service_account, service_account_secret, secret_len);
    if (jwt == NULL) {
        fprintf(stderr, "Error occured whilst making JWT for google.\n");
        goto cleanup;
    }
    printf("JWT: %s\n", jwt);
    
    cleanup:

    free(jwt);
    return NULL;
}

