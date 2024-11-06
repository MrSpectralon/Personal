
#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"
#include "../header_files/google_methods.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaration for temporary functionality.
OauthAccess* request_access_from_file(FILE** file_pointer);

static OauthAccess* spotify_credentials_from_file(FILE** file_ptr);
static OauthAccess* ytm_credentials_from_file(FILE** file_ptr);

int main (void)
{
    FILE *fptr;
    
    SpotifyPlaylist *spotify_playlist = NULL;
    
    char* temp = NULL; //Volatile only to prevent optimilizations as the content needs a guarantee of being cleared in memory.

    fptr = fopen("secrets", "r");
    fprintf(stderr, "File opened.\n");
    if (fptr == NULL)
    {
	fprintf (stderr, "Failed to open file.\n");
	goto cleanup;
    }

    OauthAccess* spotify_access = request_access_from_file(&fptr);
    if(spotify_access == NULL)
    {
        fprintf(stderr, "Failed to gain auth key.\n");
        goto cleanup;
    }
    oauth_access_print(spotify_access);

   
    OauthAccess* google_access = request_access_from_file(&fptr);
    if (google_access == NULL) {
        fprintf(stderr, "Failed to gain auth key.\n");
	goto cleanup;
    }
    oauth_access_print(google_access);
    temp = NULL;
    fclose (fptr);

    // spotify_playlist = get_spotify_playlist (spotify_access, splaylist_ID);
    // print_list_content (spotify_playlist->track_list);
    
    cleanup:

    free((char*)temp);
    oauth_access_delete(&google_access);
    oauth_access_delete (&spotify_access);
    spotify_free_playlist (&spotify_playlist);

    return 0;
}


OauthAccess* request_access_from_file(FILE** file_pointer){
    OauthAccess* access;
    const int BUFFER_LEN = 100;
    char* temp = NULL;
    char* service_str = NULL;  
    
    temp = malloc (BUFFER_LEN);
    if (temp == NULL)
    {
	fprintf (stderr, "Failed to allocate memory for temporary buffer.\n");
        goto cleanup;
    }
    
    temp[0] = '\0';
    fgets ((char*)temp, BUFFER_LEN, *file_pointer);
    service_str = malloc(strlen(temp) + 1);
    if (service_str == NULL)
    {
	fprintf (stderr, "Failed to allocate memory for service type.\n");
	goto cleanup;
    }       
    remove_new_line((char*)temp);
    service_str[0] = '\0';
    strcpy(service_str, temp);
    free(temp);
    temp = NULL;
    
    printf("Service string: %s\n", service_str); 
    if(string_compare_64b(service_str, "spotify"))
    {
        access = spotify_credentials_from_file(file_pointer);
    }
    else
    {   
	access = ytm_credentials_from_file(file_pointer);
        // auth_reply = get_auth_token_google((const char*)clientID, (const char*)clientSecret);
    }   
    free(service_str);
    service_str = NULL;
    return access;

cleanup:

    free(temp);
    free(service_str);  
    oauth_access_delete((OauthAccess**)&access);
    return NULL;
}



static OauthAccess* spotify_credentials_from_file(FILE** file_ptr)
{
    const int BUFFER_LEN = 100;
    volatile char* temp = NULL;
    volatile char* clientID = NULL;
    volatile char* clientSecret = NULL;
    char* playlist_id = NULL;
    char* auth_reply = NULL;
    OauthAccess* access = NULL;
    
    size_t secret_len = 0;
    size_t id_len = 0;
    size_t pl_len = 0;

    temp = calloc(BUFFER_LEN, sizeof(char));
    if (temp == NULL) {
	goto cleanup;
    }

    fgets ((char*)temp, BUFFER_LEN, *file_ptr);
    id_len = strlen((const char*)temp);

    clientID = malloc(id_len + 1);
    if (clientID == NULL)
    {
	fprintf (stderr, "Failed to allocate memory for clientID.\n");
	goto cleanup;
    }
    remove_new_line((char*)temp);
    clientID[0] = '\0';
    strcpy ((char*)clientID, (const char*)temp);

    fgets ((char*)temp, BUFFER_LEN, *file_ptr);
    secret_len = strlen((const char*)temp);
    clientSecret = malloc (secret_len);
    if (clientSecret == NULL)
    {
	fprintf (stderr, "Failed to allocate memory for clientSecret.\n");
	goto cleanup;
    }
    remove_new_line((char*)temp);
    clientSecret[0] = '\0';
    strcpy ((char*)clientSecret, (const char*)temp);

    memset((char*)temp, 0, BUFFER_LEN);
    fgets((char*)temp, BUFFER_LEN, *file_ptr);
    pl_len = strlen((const char*)temp);

    playlist_id = malloc(pl_len);
    if (playlist_id == NULL) {
        goto cleanup;
    }
    remove_new_line((char*)temp);
    playlist_id[0] = '\0';
    strcpy(playlist_id, (const char*)temp);
    memset ((char*)temp, '\0', strlen((char*)temp));
    free ((char*)temp);
    temp = NULL;

    auth_reply = get_auth_token_spotify((const char*)clientID, (const char*)clientSecret);
    if (auth_reply == NULL)
    {
        fprintf(stderr, "Failed to get auth reply.\n");
        goto cleanup;
    }

    memset ((char*)clientID, '\0', id_len);
    memset ((char*)clientSecret, '\0', secret_len);
    free ((char*)clientID);
    free ((char*)clientSecret);

    clientID = NULL;
    clientSecret = NULL;
    access = oauth_access_init ((const char*)auth_reply, SPOTIFY, playlist_id);
    if(access == NULL)
    {
        fprintf(stderr, "Failed to init access object.\n");
        goto cleanup;
    }
    memset (auth_reply, '\0', strlen ((const char*)auth_reply));
    free (auth_reply);
    auth_reply = NULL;



    free(playlist_id);
    playlist_id = NULL;

    return access;

  cleanup:

    if (temp != NULL)
    {
        memset((char*)temp, '\0', strlen((const char*)temp));
    }
    free((char*)temp);

    if (clientID != NULL)
    {
        memset((char*)clientID, '\0', strlen((const char*)clientID));
    }
    free((char*)clientID);
    
    if (clientSecret != NULL)
    {
        memset((char*)clientSecret, '\0', strlen((const char*)clientSecret));
    }
    free((char*)clientSecret);
    
    return NULL;
}


static OauthAccess* ytm_credentials_from_file(FILE** file_ptr)
{
    const int BUFFER_SIZE = 100; 
    
    volatile char* s_acc_key = NULL;
    size_t s_acc_key_len = 0;
    
    char* service_acc = NULL;
    size_t s_acc_len = 0;
    
    char* playlist_id = NULL;
    size_t p_id_len = 0;

    volatile char* temp = NULL;
    
    char* auth_reply = NULL;

    temp = calloc(BUFFER_SIZE, sizeof(char));
    if (temp == NULL) {
        goto cleanup;
    }
    
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    s_acc_len = strlen((char*)temp);
    service_acc = malloc(s_acc_len + 1);
    if (service_acc == NULL) {
        goto cleanup;
    }
    service_acc[0] = '\0';
    strcpy(service_acc, (char*)temp);
    
    memset((char*)temp, 0, BUFFER_SIZE); 
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    s_acc_key_len = strlen((char*)temp);
    s_acc_key = malloc(s_acc_key_len + 1);
    if (s_acc_key == NULL) {
        goto cleanup;
    }
    s_acc_key[0] = '\0';
    strcpy((char*)s_acc_key, (char*)temp);
    
    memset((char*)temp, 0, BUFFER_SIZE); 
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    p_id_len = strlen((char*)temp);
    playlist_id = malloc(p_id_len + 1);
    if (playlist_id == NULL) {
        goto cleanup;
    }
    playlist_id[0] = '\0';
    strcpy(playlist_id, (char*)temp);

    auth_reply = get_auth_token_google(service_acc, (char*)s_acc_key, s_acc_key_len);
    if (auth_reply == NULL) {
        goto cleanup;
    }
    memset((char*)temp, 0, BUFFER_SIZE);
    free((char*)temp);
    temp = NULL;

cleanup:
    if (s_acc_key != NULL) {
        memset((char*)s_acc_key, 0, s_acc_key_len);
    }
    free((char*)s_acc_key);
    if (temp != NULL) {
        memset((char*)temp, 0, BUFFER_SIZE);
    }
    free((char*)temp);
    free(auth_reply);
    free(service_acc);
    free(playlist_id);

    return NULL;
}


