#include "../header_files/playlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Track* track_init()
{
    Track* new = malloc(sizeof(Track));
    if (new == NULL) {
        fprintf(stderr, "Failed to allocate memory for track obj.\n");
        return NULL;
    }
    new->id = NULL;
    new->name = NULL;
    new->album = NULL;
    new->artist = NULL;
    new->duration = NULL;
    new->url = NULL;
    return new;
}

/*
 *	Frees up all memory allocated from the Track object.
 */
void track_free(Track **track)
{
    free((*track)->id);
    free((*track)->name);
    free((*track)->album);
    free((*track)->artist);
    free((*track)->duration);
    free((*track)->url);
    free((*track));
    *track = NULL;
}

void track_print(Track *track)
{
	printf("\tArtist: %s\n", track->artist);
	printf("\tName: %s\n", track->name);
	printf("\tAlbum: %s\n", track->album);
	printf("\tDuration WIP - Needs to convert values: %p\n", track->duration);
	printf("\tPreview URL: %s\n", track->url);
}


Playlist* playlist_init()
{
    Playlist* new = malloc(sizeof(Playlist));
    if (new == NULL) {
        fprintf(stderr, "Failed to allocate memory for Playlist obj.\n");
        return NULL;
    }
    new->service = NONE;
    new->id = NULL;
    new->name = NULL;
    new->details = NULL;
    new->total_tracks = -1;

    new->tracks = malloc(sizeof(TrackList));
    if (new->tracks == NULL) {
        fprintf(stderr, "Failed to allocate memory for initialization of TrackList obj.\n");
        free(new);
        return NULL;
    }
    new->tracks->track = NULL;
    new->tracks->next = NULL;
    new->tracks->prev = NULL;
    return new;
}

void tracklist_free(TrackList **tracklist)
{

    TrackList* tmp = NULL;
    while ((*tracklist)->next != NULL) {
        tmp = (*tracklist)->next;
        (*tracklist)->next = tmp->next;
        
        track_free(&tmp->track);
        free(tmp);
    }
    if ((*tracklist)->track != NULL)
        track_free(&(*tracklist)->track);
    free(*tracklist);
    *tracklist = NULL;
}

void playlist_free(Playlist **playlist)
{
    tracklist_free(&(*playlist)->tracks);
    free((*playlist)->id);
    free((*playlist)->name);
    free((*playlist)->details);
    free(*playlist);
    *playlist = NULL;
}

void playlist_append(Track *track, TrackList **head)
{
    if ((*head)->track == NULL) {
        (*head)->track = track;
        return;
    }

    TrackList* new = malloc(sizeof(TrackList));
    if (new == NULL) {
        fprintf(stderr, "Failed to  allocate memory for new TrackList node.\n");
        return;
    }
    
    new->track = track;
    new->next = *head;
    new->prev = ((*head)->prev == NULL) ? NULL : (*head)->prev;
    (*head)->prev = new;
    *head = new;
}

void playlist_pop(TrackList **node)
{
    TrackList* tmp = *node;
    if (tmp->next != NULL) {
        tmp->next->prev = tmp->prev;
    }
    
    if(tmp->prev != NULL){
        tmp->prev->next = tmp->next;
    }
    track_free(&tmp->track);
    free(tmp);
    *node = NULL;
}
