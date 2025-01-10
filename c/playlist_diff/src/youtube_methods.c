#include "../header_files/youtube_methods.h"
#include "../header_files/curl_handler.h"

#include <cjson/cJSON.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

char* create_JWT(const char* service_account, const char* user_email, const char* service_acc_private_key, const size_t key_len)
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
    //   "aud": "user-you-want_access-to@gmail.com",
    //   "scope": https://www.googleapis.com/auth/youtube",
    //   "aud": "https://oauth2.googleapis.com/token",
    //   "exp": <integer>unix time + iat,
    //   "iat": unix time
    // }

    time_t iat = time(NULL);
    time_t exp = iat + 3600;
    

    cJSON_AddStringToObject(jwt_payload, "iss", service_account);
    cJSON_AddStringToObject(jwt_payload, "sub", user_email);
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
    free(jwt);
    free(signature);
    free(jwt_h_str);
    free(header_b64);
    free(jwt_p_str);
    free(payload_b64);

    return NULL;
    

}


char* get_auth_token_google (const char* service_account, const char* service_account_secret, const size_t secret_len)
{
    char *curlPostField = NULL;
    char user_email[] = "stian.pedersen1996@gmail.com";
    char* jwt = NULL;
    jwt = create_JWT(service_account, user_email, service_account_secret, secret_len);
    if (jwt == NULL) {
        fprintf(stderr, "Error occured whilst making JWT for google.\n");
        goto cleanup;
    }
    


    const char grantTypeAndClientID[] = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=";
    const char content_type[] = "Content-Type: application/x-www-form-urlencoded";
    const char destination_url[] = "oauth2.googleapis.com";

    long fieldSize = strlen (grantTypeAndClientID) + strlen(jwt);

    curlPostField = calloc(fieldSize + 1, sizeof (char));
    if (curlPostField == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for curl post field.\n");
        return NULL;
    }
    strcpy(curlPostField, grantTypeAndClientID);
    strcat (curlPostField, jwt);
    printf("\n\n%s\n\n", jwt);
    char* response = curl_post_request(destination_url, content_type, curlPostField, fieldSize);
    if (response == NULL) {
        goto cleanup;
    }
    free(curlPostField);
    curlPostField = NULL;
    free(jwt);
    jwt = NULL;
    return response;
    
cleanup:
    
    free(curlPostField);
    free(jwt);
    return NULL;
}

/**
 * Returns 0 if no errors occured.
 * Returns 1 if an error occured.
**/
int parce_youtube_playlist_details(Playlist** playlist, char* data)
{
    
    cJSON* json = cJSON_Parse(data);
    if (json == NULL)
    {
        fprintf(stderr, "Failed to parce youtube playlist data.\n");
        return 1;
    }
    cJSON* items = cJSON_GetObjectItem(json, "items");
    if (items == NULL) {
        fprintf(stderr, "Could not fetch items from youtube JSON object.\n");
        goto cleanup;
    }
    cJSON* items_content = cJSON_GetArrayItem(items, 0);
    if (items_content == NULL) {
        fprintf(stderr, "Could not fetch index 0 from youtube items JSON object.\n");
        goto cleanup;
    }
    cJSON* snippet = cJSON_GetObjectItem(items_content, "snippet");
    if (snippet == NULL) {
        fprintf(stderr, "Could not fetch snippet from JSON object.\n");
        goto cleanup;
    }
    cJSON* content_details = cJSON_GetObjectItem(items_content, "contentDetails");
    if (content_details == NULL) {
        fprintf(stderr, "Could not fetch contentDetails from JSON object.\n");
        goto cleanup;
    }

    cJSON* title = cJSON_GetObjectItem(snippet, "title");
    if(title == NULL )
    {
        fprintf(stderr, "Could not get title string from snippet obj.\n");
        goto cleanup;
    }
    cJSON* description = cJSON_GetObjectItem(snippet, "description");
    if(description == NULL )
    {
        fprintf(stderr, "Could not get description string from snippet obj.\n");
        goto cleanup;
    }
    cJSON* item_count = cJSON_GetObjectItem(content_details, "itemCount");
    if(item_count == NULL )
    {
        fprintf(stderr, "Could not get item count from content details obj.\n");
        goto cleanup;
    }

    (*playlist)->name = strdup(title->valuestring);
    if((*playlist)->name == NULL )
    {
        fprintf(stderr, "Could not duplicate name string.\n");
        goto cleanup;
    }
    (*playlist)->description = strdup(description->valuestring);
    if((*playlist)->description == NULL )
    {
        fprintf(stderr, "Could not duplicate description string.\n");
        goto cleanup;
    }
    (*playlist)->total_tracks = item_count->valueint; 
    
    cJSON_Delete(json);
    return 0;
cleanup:
    cJSON_Delete(json);
    return 1;
}

char* get_youtube_playlist_details(OauthAccess* access)
{

    char url_api_destination[] = "https://youtube.googleapis.com/youtube/v3/playlists?part=snippet,contentDetails&fields=items(snippet(title,description),contentDetails)&id=";
    
    char auth[] = "Authorization: Bearer ";
    char* authorization = NULL;
    int authorization_len = 0;
    char* request_url = NULL;
    int request_url_len = 0;
    
    char* reply = NULL;


    request_url_len = strlen(url_api_destination) + strlen(access->playlist);
    request_url = malloc(request_url_len + 1);
    if (request_url == NULL) {
        return NULL;
    }
    request_url[0] = '\0';
    strcat(request_url, url_api_destination);
    strcat(request_url, access->playlist);
    
    authorization_len = strlen(auth) + strlen(access->token);
    authorization = malloc(authorization_len + 1);
    if (authorization == NULL) {
        goto cleanup;
    }
    authorization[0] = '\0';
    strcat(authorization, auth);
    strcat(authorization, access->token);

    reply = curl_get_request(authorization, request_url);
    cleanup:
    free(authorization);
    free(request_url);
    return reply;
}

Playlist* get_youtube_playlist(OauthAccess* access)
{   
    char* playlist_details_json = NULL;

    char* tracks_data = NULL;
    char* next_page = NULL;
    
    int tracks_recieved = 0;

    Playlist* playlist = playlist_init();
    if (playlist == NULL) return NULL;

    playlist->id = strdup(access->playlist);
    if (playlist->id == NULL) {
        fprintf(stderr, "Failed to allocate youtube id in object.\n");
        goto cleanup;
    }

    playlist_details_json = get_youtube_playlist_details(access);
    if (playlist_details_json == NULL) {
        goto cleanup;
    }
    if (parce_youtube_playlist_details(&playlist, playlist_details_json)) goto cleanup;

    while (tracks_recieved < playlist->total_tracks) {
        tracks_data = get_youtube_playlist_tracks(access, next_page);
        if (tracks_data == NULL) {
            fprintf(stderr, "Failed to recieve youtube playlist tracks.\n");
        }
        free(next_page);
        next_page = NULL;
        int temp = parce_youtube_playlist_tracks(&playlist->tracks, tracks_data, &next_page);
        if (!temp) {
            fprintf(stderr, "Parced 0 tracks from youtube.\n");
            goto cleanup;
        }
        free(tracks_data);
        tracks_data = NULL;
        tracks_recieved += temp;
        printf ("\rReceved and parced %d of %d songs.\n", tracks_recieved, playlist->total_tracks);
	fflush (stdout);
    }
    
    free(tracks_data);
    free(next_page);
    free(playlist_details_json);

    return playlist;
  cleanup:

    free(next_page);
    free(playlist_details_json);
    free(tracks_data);
    playlist_free(&playlist);
    return  NULL;
}

char* get_youtube_playlist_tracks(OauthAccess* access, char* page_token)
{
    char url_api_destination[] = "https://youtube.googleapis.com/youtube/v3/playlistItems?part=snippet&maxResults=50&fields=nextPageToken,pageInfo(resultsPerPage),items(snippet(title,videoOwnerChannelTitle,resourceId(videoId)))&playlistId=";
    char p_t[] = "&page_token=";
    
    char auth[] = "Authorization: Bearer ";
    
    char* authorization = NULL;
    int authorization_len = 0;
    char* request_url = NULL;
    int request_url_len = 0;
    
    char* next_page_token = NULL;

    char* reply = NULL;

    if (page_token != NULL) {
        request_url_len = strlen(p_t) + strlen(page_token);
        next_page_token = malloc(request_url_len + 1);
        next_page_token[0] = '\0';
        strcat(next_page_token, p_t);
        strcat(next_page_token, page_token);
    }

    request_url_len += strlen(url_api_destination) + strlen(access->playlist);
    request_url = malloc(request_url_len + 1);
    if (request_url == NULL) {
        return NULL;
    }
    request_url[0] = '\0';
    strcat(request_url, url_api_destination);
    strcat(request_url, access->playlist);
    
    if (next_page_token != NULL) strcat(request_url, next_page_token);


    authorization_len = strlen(auth) + strlen(access->token);
    authorization = malloc(authorization_len + 1);
    if (authorization == NULL) {
        goto cleanup;
    }
    authorization[0] = '\0';
    strcat(authorization, auth);
    strcat(authorization, access->token);

    reply = curl_get_request(authorization, request_url);
    free(authorization);
    free(request_url);
    free(next_page_token);
    return reply;


cleanup:
    free(request_url);
    free(authorization);
    free(next_page_token);

    return NULL;
}

int parce_youtube_playlist_tracks(TrackList** track_list, char* track_data, char** next_page)
{
    int tracks_parced = 0;
    cJSON* json = cJSON_Parse(track_data);
    if (json == NULL) goto cleanup;

    cJSON* page_token = cJSON_GetObjectItem(json, "nextPageToken");
    if (page_token == NULL) {
        *next_page = NULL;
    }
    else {
        *next_page = strdup(page_token->valuestring);
        if (*next_page == NULL)
        {
            fprintf(stderr, "Failed to duplicate page token.\n");
            goto cleanup;
        }
    }
    
    cJSON* items = cJSON_GetObjectItem(json, "items");
    tracks_parced = cJSON_GetArraySize(items);
    if(items == NULL) goto cleanup;
    cJSON* item = NULL;
    
    cJSON_ArrayForEach(item, items){
        cJSON* snippet = cJSON_GetObjectItem(item, "snippet");
        if (snippet == NULL) continue;
        
        cJSON* resource_id = cJSON_GetObjectItem(snippet, "resourceId");
        if (resource_id == NULL) {
            fprintf(stderr, "Error getting resource ID from YT-playlist JSON.\n");
            goto cleanup;
        }
        cJSON* video_id = cJSON_GetObjectItem(resource_id, "videoId");
        if (video_id == NULL) {
            fprintf(stderr, "Error getting video ID from YT-playlist JSON.\n");
            goto cleanup;
        }
        cJSON* channel = cJSON_GetObjectItem(snippet, "videoOwnerChannelTitle");
        if (channel == NULL) {
            fprintf(stderr, "Error getting channel name from YT-playlist JSON.\n");
            goto cleanup;
        }
        cJSON* title = cJSON_GetObjectItem(snippet, "title");
        if (title == NULL) {
            fprintf(stderr, "Error getting video title from YT-playlist JSON.\n");
            goto cleanup;
        }
        

        Track* new_track = track_init();
        if (new_track == NULL) goto cleanup;
        
        new_track->id = strdup(video_id->valuestring);
        new_track->name = strdup(title->valuestring);
        new_track->artist = strdup(channel->valuestring);
        playlist_append(new_track, track_list);

    }

    cJSON_Delete(json);
    return tracks_parced;
cleanup:

    cJSON_Delete(json);
    return 0;
}

