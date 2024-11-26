#include "../header_files/spotify_track_list.h"



int add_playlist_info (const char *name, const char *description,
		   const int number_of_tracks)
{
    return 0;
}

/**
 * Appends track data into a linked list of spotify tracks.
 */
int append_spotify_track (ListSpotifyTracks **list_head,
		      const char *name,
		      const char *album,
		      const char *artist,
		      const int duration,
		      const char *preview_url, const char *external_url)
{

    const char NOT_ADDED[] = "NA";
    SpotifyTrack *new_track = malloc (sizeof (SpotifyTrack));
    if (new_track == NULL)
    {
        fprintf (stderr, "Error when allocating new track object.\n");
        return 0;
    }


    new_track->name = NULL;
    new_track->album = NULL;
    new_track->artist = NULL;
    new_track->preview_url = NULL;
    new_track->external_url = NULL;

    new_track->duration_ms = duration;

    new_track->name = strndup (name, strlen (name));
    if (new_track->name == NULL)
    {
        fprintf (stderr,
            "Error allocating memory for Spotify list content.\n");
        goto cleanup;
    }

    new_track->album = strndup (album, strlen (album));
    if (new_track->album == NULL)
    {
        fprintf (stderr,
            "Error allocating memory for Spotify list content.\n");
        goto cleanup;
    }

    new_track->artist = strndup (artist, strlen (artist));
    if (new_track->artist == NULL)
    {
        fprintf (stderr,
            "Error allocating memory for Spotify list content.\n");
        goto cleanup;
    }

    //Some songs don't have a preview and is thus NULL.
    if (preview_url != NULL)
    {
        new_track->preview_url = strndup (preview_url, strlen (preview_url));
        if (new_track->preview_url == NULL)
        {
            fprintf (stderr,
                    "Error allocating memory for Spotify list content.\n");
            goto cleanup;
        }
    }
    else
    {
        new_track->preview_url = strndup (NOT_ADDED, strlen (NOT_ADDED));
        if (new_track->preview_url == NULL)
        {
            fprintf (stderr,
                    "Error allocating memory for Spotify list content.\n");
            goto cleanup;
        }
    }

    //Same as above.
    if (external_url != NULL)
    {
        new_track->external_url = strndup (external_url, strlen (external_url));
        if (new_track->external_url == NULL)
        {
            fprintf (stderr,
                    "Error allocating memory for Spotify list content.\n");
            goto cleanup;
        }
    }
    else
    {
        new_track->external_url = strndup (NOT_ADDED, strlen (NOT_ADDED));
        if (new_track->external_url == NULL)
        {
            fprintf (stderr,
                    "Error allocating memory for Spotify list content.\n");
            goto cleanup;
        }
    }

    ListSpotifyTracks *new_obj = calloc(1, sizeof (ListSpotifyTracks));
    if (new_obj == NULL)
    {
        fprintf (stderr, "Error when allocating new list node.\n");
        goto cleanup;
    }

    new_obj->track = new_track;

    if (*list_head == NULL)
    {
        printf ("Empty list - Adding from NULL.\n");
        new_obj->prev = NULL;
        new_obj->next = NULL;
        *list_head = new_obj;
    }
    else
    {
        new_obj->next = *list_head;
        (*list_head)->prev = new_obj;
        *list_head = new_obj;
    }

    return 1;

    //Section is only run if errors occur during allocations.
    cleanup:

    free (new_track->name);
    new_track->name = NULL;
    free (new_track->album);
    new_track->album = NULL;
    free (new_track->artist);
    new_track->artist = NULL;
    free (new_track->preview_url);
    new_track->preview_url = NULL;
    free (new_track->external_url);
    new_track->external_url = NULL;

    free (new_obj);
    new_obj = NULL;
    return 0;
}

/**
 * Deletes the entire linked list from memory.
 * Deletes as in free up memory.
 */
void spotify_free_tracklist (ListSpotifyTracks **list_head)
{
    ListSpotifyTracks *temp = *list_head;
    if (temp == NULL)
    {
        return;
    }

    while (temp->next != NULL)
    {
        pop_spotify_track (temp->next, *list_head);
    }
    pop_spotify_track (temp, *list_head);
}


/**
 * Deletes a single track from the linked list.
 */
void pop_spotify_track (ListSpotifyTracks *track_node, ListSpotifyTracks *list_head)
{
    if (track_node == NULL)
    {
        fprintf (stderr, "Attempting to pop a track from empty node.\n");
        return;
    }
    //Check if this is first element
    if (track_node->prev == NULL)
    {
        //check if this is last element.
        if (track_node->next == NULL)
        {
            //If last - reset list.
            list_head = NULL;
        }
        else
        {
            //Not last, move head.
            list_head = track_node->next;
            track_node->next->prev = NULL;
        }

    }
    else
    {
        //Check if this is last element
        if (track_node->next == NULL)
        {
            track_node->prev->next = NULL;
        }
        else
        {
            track_node->prev->next = track_node->next;
            track_node->next->prev = track_node->prev;
        }
    }
    free_spotify_track (&track_node->track);
    free (track_node);
    track_node = NULL;
}

/**
 * Frees the data in a spotify track.
 */
void free_spotify_track (SpotifyTrack **track)
{
    if (!*track)
	return;

    free ((*track)->name);
    free ((*track)->album);
    free ((*track)->artist);
    free ((*track)->preview_url);
    free ((*track)->external_url);
    free ((*track));
    *track = NULL;
}

/**
 * Prints all tracks in the track list in a somewhat presentable way.
 */
void print_list_content (ListSpotifyTracks *list_head)
{
    ListSpotifyTracks *temp = list_head;

    int i = 1;
    while (temp->track != NULL)
    {
        printf ("\nTrack #%d\n", i);
        printf ("\tName: %s, %d\n", temp->track->name,
            temp->track->duration_ms);
        printf ("\tAlbum: %s\n", temp->track->album);
        printf ("\tArtist: %s\n", temp->track->artist);
        printf ("\tPreview URL: %s\n", temp->track->preview_url);
        printf ("\tExternal URL: %s\n", temp->track->external_url);
        i++;
        temp = temp->next;
    }
}


/**
 * Should look for identical tracks.
 * Not sure if i've used it or even tested this function yet.
 */
ListSpotifyTracks* find_same_track (ListSpotifyTracks *list_head, const SpotifyTrack *track)
{
    ListSpotifyTracks *temp = list_head;

    while (temp != NULL)
    {
        if (same_song (temp->track, track))
        {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}


/**
 * WIP
 */
int same_song (const SpotifyTrack *track1, const SpotifyTrack *track2)
{
    return 0;
}


/**
 * Initializes the linked list of spotify tracks.
 */
SpotifyPlaylist* spotify_playlist_init()
{
    SpotifyPlaylist *playlist = malloc (sizeof (SpotifyPlaylist));
    if (playlist == NULL)
    {
        fprintf (stderr, "Error when allocating memory for playlist.\n");
        goto cleanup;
    }
    playlist->track_list = malloc (sizeof (ListSpotifyTracks));
    if (playlist->track_list == NULL)
    {
        fprintf (stderr, "Error allocating memory for list of tracks.\n");
        goto cleanup;
    }

    playlist->name = NULL;
    playlist->description = NULL;
    playlist->total_tracks = 0;
    playlist->track_list->next = NULL;
    playlist->track_list->prev = NULL;
    playlist->track_list->track = NULL;

    return playlist;

     cleanup:
    free (playlist->track_list);
    free (playlist);
    playlist = NULL;
    return NULL;
}


/**
 * Deletes the entire playlist from memory.
 */
void spotify_free_playlist (SpotifyPlaylist **playlist)
{
    if (*playlist == NULL)
    {
        return;
    }
    if ((*playlist)->track_list == NULL)
    {
        fprintf (stderr,
            "Tried to free non initialized list of tracks in playlist.\n");
        return;
    }
    spotify_free_tracklist (&(*playlist)->track_list);
    free ((*playlist)->name);
    (*playlist)->name = NULL;
    free ((*playlist)->description);
    (*playlist)->description = NULL;
    free (*playlist);
    *playlist = NULL;
}
