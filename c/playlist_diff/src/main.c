
#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"
#include "../header_files/youtube_methods.h"
#include "../header_files/youtube_playlist.h"
#include <curl/curl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaration for temporary functionality.
OauthAccess* request_access_from_file(FILE** file_pointer);

static OauthAccess* spotify_credentials_from_file(FILE** file_ptr);
static OauthAccess* ytm_credentials_from_file(FILE** file_ptr);

void test();

int main (void)
{
    FILE* fptr = NULL;
    OauthAccess* google_access = NULL;
    SpotifyPlaylist* spotify_playlist = NULL;
    OauthAccess* spotify_access = NULL;
    YoutubePlaylist* youtube_playlist = NULL;

    char* temp = NULL; //Volatile only to prevent optimilizations as the content needs a guarantee of being cleared in memory.
    

    // test();
    // goto cleanup;

    fptr = fopen("hidden_from_git/secrets", "r");
    fprintf(stderr, "File opened.\n");
    if (fptr == NULL)
    {
	fprintf (stderr, "Failed to open file.\n");
	goto cleanup;
    }

    spotify_access = request_access_from_file(&fptr);
    if(spotify_access == NULL)
    {
        fprintf(stderr, "Failed to gain auth key.\n");
        goto cleanup;
    }
    oauth_access_print(spotify_access);

	//
	//    google_access = request_access_from_file(&fptr);
	//    if (google_access == NULL) {
	//        fprintf(stderr, "Failed to gain auth key.\n");
	// goto cleanup;
	//    }
	//    oauth_access_print(google_access);
    temp = NULL;
    fclose (fptr);
    
    // youtube_playlist = get_youtube_playlist(google_access);
    // yt_playlist_print(youtube_playlist);
    

    spotify_playlist = get_spotify_playlist(spotify_access);
    // print_spotify_playlist(spotify_playlist);

    

    cleanup:

    free((char*)temp);
    oauth_access_delete(&google_access);
    oauth_access_delete (&spotify_access);
    spotify_free_playlist (&spotify_playlist);
    yt_playlist_free(&youtube_playlist);
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
    
    const int BUFFER_SIZE = 550;
    volatile char* temp = NULL;
    volatile char* clientID = NULL;

    // Shall be redirect url (temporarily™)
    volatile char* clientSecret = NULL;
    char* playlist_id = NULL;
    char* auth_reply = NULL;
    OauthAccess* access = NULL;
    
    // size_t secret_len = 0;
    // size_t id_len = 0;
    // size_t pl_len = 0;

    temp = calloc(BUFFER_SIZE, sizeof(char));
    if (temp == NULL) {
        goto cleanup;
    }
    
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    clientID = strdup((char*)temp);
    if (clientID == NULL) {
        goto cleanup;
    }
    
    memset((char*)temp, 0, BUFFER_SIZE); 
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    clientSecret = strdup((char*)temp);
    if (clientSecret == NULL) {
        goto cleanup;
    }

    memset((char*)temp, 0, BUFFER_SIZE); 
    fgets((char*)temp, BUFFER_SIZE, *file_ptr);
    remove_new_line((char*)temp);
    playlist_id = strdup((char*)temp);
    if (playlist_id == NULL) {
        goto cleanup;
    }

    //TODO: Make webserver to handle this step.
    // auth_reply = get_auth_token_google(Some parameter.);

    memset((char*)temp, 0, BUFFER_SIZE); 
    printf("Paste in entire reply json: ");
    if (!fgets((char*)temp, BUFFER_SIZE, stdin)) goto cleanup;
    auth_reply = strdup((char*)temp);
    if (auth_reply == NULL) goto cleanup;

    memset((char*)temp, 0, BUFFER_SIZE);
    
    free((char*)clientID);
    free((char*)clientSecret);
    free((char*)temp);
    temp = NULL;
    clientID = NULL;
    clientSecret = NULL;
    
    access = oauth_access_init(auth_reply, YOUTUBE, playlist_id);
    if (access == NULL) {
        fprintf(stderr, "Failed to initialize access object");
        goto cleanup;
    }
    free(auth_reply);
    auth_reply = NULL;
    
    free(playlist_id);
    playlist_id = NULL;

    return access;

cleanup:
    if (clientID != NULL) {
        memset((char*)clientID, 0, strlen((char*)clientID));
    }
    if (temp != NULL) {
        memset((char*)temp, 0, BUFFER_SIZE);
    }
    free((char*)temp);
    free((char*)clientSecret);
    free(auth_reply);
    free(playlist_id);

    return NULL;
}

char* get_usr_input(char* print_msg)
{
    char buffer[600];
    printf("%s", print_msg); 
    scanf("%s", buffer);
    size_t s_len = strlen(buffer);
    char* temp = calloc(s_len + 1, sizeof(char));
    if (temp == NULL)
    {
       return NULL; 
    }
    strcpy(temp, buffer);

    return temp;
}

void test()
{

    char* play_name = NULL;
    char* play_desc = NULL;
    char* play_id = NULL;

    char* name = NULL; 
    char* description = NULL; 
    char* duration = NULL; 
    char* artist = NULL; 
    char* id = NULL;

    YoutubePlaylist* ytp = NULL;

    char tmp[2];
    ytp = malloc(sizeof(YoutubePlaylist));
    if (ytp == NULL) {
        goto end;
    }
    yt_playlist_init(&ytp);

    printf("P - Set playlist data.\n");
    printf("T - Add track data.\n");
    printf("L - List all added tracks.\n");
    printf("D - Delete all tracks.\n");
    printf("Q - Quit.\n");
    while (1) {
        printf("OPTION: ");
        scanf("%s", tmp);
        
        switch(tmp[0])
        {
            case 'P':
                free(play_name);free(play_desc); free(play_id);

                printf("Playlist:\n");
                play_name = get_usr_input("\tName: ");
                if(play_name == NULL) goto end; 
                play_desc = get_usr_input("\tDescription: ");
                if(play_desc == NULL) goto end; 
                play_id = get_usr_input("\tID: ");
                if(play_id == NULL) goto end; 
                
                free(ytp->name); free(ytp->description); free(ytp->id); 
                ytp->name = strdup(play_name);
                if (ytp->name == NULL) goto end;
                ytp->description = strdup(play_desc);
                if (ytp->description == NULL) goto end;
                ytp->id = strdup(play_id);
                if (ytp->id == NULL) goto end;

                break;

            case 'T':
                free(name); free(description); free(duration); free(artist); free(id);
                printf("Song:\n");
                name = get_usr_input("\tName: ");
                if(name == NULL) goto end; 
                artist = get_usr_input("\tArtist: ");
                if(artist == NULL) goto end; 
                description = get_usr_input("\tDescription: ");
                if(description == NULL) goto end; 
                duration = get_usr_input("\tDuration: ");
                if(duration == NULL) goto end; 
                id = get_usr_input("\tID: ");
                if(id == NULL) goto end; 
                
                YtTrack* new_track = yt_track_create(id, name, artist, description, duration);
                if (new_track == NULL) {
                    goto end;
                }
                yt_track_list_append(&ytp->track_list_head, new_track);
                break;
            
            case 'L':
                yt_playlist_print(ytp);
                break;
            
            case 'D':
                yt_track_list_free(&(ytp->track_list_head));
                break;

            case 'H':
                printf("P - Set playlist data.\n");
                printf("T - Add track data.\n");
                printf("L - List all added tracks.\n");
                printf("D - Delete all tracks.\n");
                printf("Q - Quit.\n");
                break;
            case 'Q':
                goto end;
            default:
                break;
        }

    }

end:

    free(play_name); 
    free(play_desc); 
    free(play_id); 

    free(name); 
    free(description); 
    free(duration);
    free(artist); 
    free(id);
    yt_playlist_free(&ytp);
}

