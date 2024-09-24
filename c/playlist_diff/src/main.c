#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"

//File pointer
FILE *fptr;

int main (void)
{
    SpotifyAccess *access = NULL;
    SpotifyPlaylist *spotify_playlist = NULL;

    char *clientID;
    char *clientSecret;
    char *playlist_ID;
    char *temp;


    fptr = fopen ("secrets", "r");
    if (fptr == NULL)
	{
		fprintf (stderr, "Failed to open file.\n");
		goto cleanup;
	}

    temp = malloc (100);

    fgets (temp, 100, fptr);
    clientID = malloc (strlen (temp));
    if (clientID == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for clientID.\n");
		goto cleanup;
    }

    removeNewline (temp);
    strcpy (clientID, temp);

    fgets (temp, 100, fptr);
    clientSecret = malloc (strlen (temp));
    if (clientSecret == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for clientSecret.\n");
		goto cleanup;
	}
    removeNewline (temp);
    strcpy (clientSecret, temp);

    fgets (temp, 100, fptr);
    playlist_ID = malloc (strlen (temp));
    if (playlist_ID == NULL)
	{
		fprintf (stderr, "Failed to allocate memory for playlist ID.\n");
		goto cleanup;
	}
    removeNewline (temp);
    strcpy (playlist_ID, temp);

    fclose (fptr);
    fptr = NULL;
    memset (temp, '\0', strlen (temp));
    free (temp);
    temp = NULL;

    char *auth_reply = getAuthTokenSpotify (clientID, clientSecret);

    memset (clientID, '\0', strlen (clientID));
    memset (clientSecret, '\0', strlen (clientSecret));
    free (clientID);
    free (clientSecret);

    access = spotify_access_init (auth_reply);
    memset (auth_reply, '\0', strlen (auth_reply));
    free (auth_reply);

    spotify_playlist = get_spotify_playlist (access, playlist_ID);
    print_list_content (spotify_playlist->track_list);


	cleanup:
    free (playlist_ID);
    spotify_access_delete (&access);
    spotify_free_playlist (&spotify_playlist);

    return 0;
}
