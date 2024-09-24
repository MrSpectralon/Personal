
#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"
#include "../header_files/google_methods.h"

//File pointer

OauthAccess* request_access_from_file(FILE** file_pointer);

int main (void)
{
    FILE *fptr;
    // OauthAccess* google_access = NULL;
    SpotifyPlaylist *spotify_playlist = NULL;
    char* temp = NULL;
    char *playlist_ID;

    fptr = fopen("secrets", "r");
    fprintf(stderr, "File opened.\n");
    if (fptr == NULL)
	{
		fprintf (stderr, "Failed to open file.\n");
		goto cleanup;
	}

    OauthAccess* spotify_access = request_access_from_file(&fptr);
    oauth_access_print(spotify_access);
    if(spotify_access == NULL)
    {
        fprintf(stderr, "Failed to gain auth key.\n");
        goto cleanup;
    }

    temp = malloc(100);
    if (temp == NULL)
    {
        fprintf(stderr, "Error allocating memory for buffer.\n");
    }
    temp[0] = '\0';

    fgets(temp, 100, fptr);
    playlist_ID = malloc (strlen (temp));
    if (playlist_ID == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for playlist ID.\n");
		goto cleanup;
	}
    remove_new_line (temp);
    strcpy (playlist_ID, temp);
    fclose (fptr);
    spotify_playlist = get_spotify_playlist (spotify_access, playlist_ID);
    print_list_content (spotify_playlist->track_list);
    

	cleanup:
    free (playlist_ID);
    oauth_access_delete (&spotify_access);
    spotify_free_playlist (&spotify_playlist);

    return 0;
}


OauthAccess* request_access_from_file(FILE** file_pointer){
    volatile OauthAccess* access;

    volatile char* temp = NULL;
    char* service_str = NULL;  
    volatile char* clientID = NULL;
    volatile char* clientSecret = NULL;
    volatile char* auth_reply = NULL; 
    Service service;
    temp = malloc (100);
    if (temp == NULL)
    {
		fprintf (stderr, "Failed to allocate memory for temporary buffer.\n");
        goto cleanup;
    }
    temp[0] = '\0';
    fgets ((char*)temp, 100, *file_pointer);
    service_str = malloc (strlen ((const char*)temp));
    if (service_str == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for service type.\n");
		goto cleanup;
    }       
    remove_new_line((char*)temp);
    service_str[0] = '\0';
    strcpy(service_str, (const char*)temp);

    temp[0] = '\0';
    fgets ((char*)temp, 100, *file_pointer);
    clientID = malloc (strlen ((const char*)temp));
    if (clientID == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for clientID.\n");
		goto cleanup;
    }
    remove_new_line ((char*)temp);
    clientID[0] = '\0';
    strcpy ((char*)clientID, (const char*)temp);

    fgets ((char*)temp, 100, *file_pointer);
    clientSecret = malloc (strlen ((const char*)temp));
    if (clientSecret == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for clientSecret.\n");
		goto cleanup;
	}
    remove_new_line ((char*)temp);
    clientSecret[0] = '\0';
    strcpy ((char*)clientSecret, (const char*)temp);

    memset ((char*)temp, '\0', strlen ((const char*)temp));
    free ((char*)temp);
    temp = NULL;

    if(string_compare_64b((const char*)service_str, "spotify"))
    {
        service = SPOTIFY;
        auth_reply = get_auth_token_spotify((const char*)clientID, (const char*)clientSecret);
    }
    else
    {   
        service = YOUTUBE;
        auth_reply = get_auth_token_google((const char*)clientID, (const char*)clientSecret);
    }

    if (auth_reply ==NULL)
    {
        fprintf(stderr, "Failed to get auth reply.\n");
        goto cleanup;
    }
    

    memset ((char*)clientID, '\0', strlen ((const char*)clientID));
    memset ((char*)clientSecret, '\0', strlen ((const char*)clientSecret));
    free ((char*)clientID);
    free ((char*)clientSecret);
    clientID = NULL;
    clientSecret = NULL;

    access = oauth_access_init ((const char*)auth_reply, service);
    if(access == NULL)
    {
        fprintf(stderr, "Failed to init access object.\n");
        goto cleanup;
    }
    
    memset ((char*)auth_reply, '\0', strlen ((const char*)auth_reply));
    free ((char*)auth_reply);
    auth_reply = NULL;

    return (OauthAccess*)access;

    cleanup:
    free(service_str);  
    oauth_access_delete((OauthAccess**)&access);
    memset((char*)temp, '\0', strlen((const char*)temp));
    free((char*)temp);
    memset((char*)clientID, '\0', strlen((const char*)clientID));
    free((char*)clientID);
    memset((char*)clientSecret, '\0', strlen((const char*)clientSecret));
    free((char*)clientSecret);
    memset((char*)auth_reply, '\0', strlen((const char*)auth_reply));
    free((char*)auth_reply); 

    return NULL;
}

