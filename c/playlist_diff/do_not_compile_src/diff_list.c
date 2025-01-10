#include "../header_files/spotify_track_list.h"
#include "../header_files/youtube_playlist.h"
#include "../header_files/diff_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static void search_copy_list_youtube(ListCopy** list_copy, YoutubePlaylist* playlist);
static void search_copy_list_spotify(ListCopy** list_copy, SpotifyPlaylist* playlist);


int diff_tracks_list_append_spotify(TracksList** diff_list_head, ListSpotifyTracks* list_node)
{
    if ((*diff_list_head)->tracks == NULL) {
        (*diff_list_head)->tracks = malloc(sizeof(Tracks));
        if ((*diff_list_head)->tracks == NULL) {
            fprintf(stderr, "Allocating memory for Tracks failed.\n");
            return 0;
        }
        (*diff_list_head)->tracks->track1 = list_node->track;
        (*diff_list_head)->tracks->track2 = NULL;
        (*diff_list_head)->tracks->status = NIL;
        return 1;
    }
    
    TracksList* new = malloc(sizeof(TracksList));
    if (new == NULL) {
        fprintf(stderr, "Allocating memory for new TracksList obj failed.\n");
        return 0;
    }
    new->tracks = malloc(sizeof(Tracks));
    if (new->tracks == NULL) {
        fprintf(stderr, "Allocating memory for Tracks failed.\n");
        free(new);
        return 0;
    }
    new->tracks->track2 = NULL;
    new->tracks->status = NIL;

    new->tracks->track1 = list_node->track;
    new->next = *diff_list_head;
    (*diff_list_head)->prev = new;
    new->prev = NULL;
    *diff_list_head = new;
    return 1;
}


int diff_tracks_list_append_youtube(TracksList** diff_list_head, YoutubeTrackList* list_node)
{
    if ((*diff_list_head)->tracks == NULL) {
        (*diff_list_head)->tracks = malloc(sizeof(Tracks));
        if ((*diff_list_head)->tracks == NULL) {
            fprintf(stderr, "Allocating memory for Tracks failed.\n");
            return 0;
        }
        (*diff_list_head)->tracks->track1 = list_node->track;
        (*diff_list_head)->tracks->track2 = NULL;
        (*diff_list_head)->tracks->status = NIL;
        return 1;
    }
    
    TracksList* new = malloc(sizeof(TracksList));
    if (new == NULL) {
        fprintf(stderr, "Allocating memory for new TracksList obj failed.\n");
        return 0;
    }
    new->tracks = malloc(sizeof(Tracks));
    if (new->tracks == NULL) {
        fprintf(stderr, "Allocating memory for Tracks failed.\n");
        free(new);
        return 0;
    }
    new->tracks->track2 = NULL;
    new->tracks->status = NIL;

    new->tracks->track1 = list_node->track;
    new->next = *diff_list_head;
    (*diff_list_head)->prev = new;
    new->prev = NULL;
    *diff_list_head = new;
    return 1;
}


void diff_list_append_playlist1_to_trackslist(DiffList* diff)
{
    if (diff->list != NULL) {
        printf("Cannot copy tracklist when diff list is not empty.\n");
        return;
    }

    //Copying a pointer to first playlist - used as reference for later functionality.
    if (diff->service1 == YOUTUBE) {
        YoutubePlaylist* temp = (YoutubePlaylist*)diff->playlist1;
        
        while (temp->track_list_head != NULL) {
            if (!diff_tracks_list_append_youtube(&diff->list, temp->track_list_head)) {
                return;
            }
            temp->track_list_head = temp->track_list_head->next;
        }
    }
    else {
        SpotifyPlaylist* temp = (SpotifyPlaylist*)diff->playlist1;
        while (temp->track_list != NULL) {
            if (!diff_tracks_list_append_spotify(&diff->list, temp->track_list)) {
                return;
            }
            temp->track_list = temp->track_list->next;

        }

    }

}


void diff_list_make_search_copy(DiffList* diff)
{
    if (diff->search_copy != NULL){
        fprintf(stderr, "Search copy list has already been made.\n");
        return;
    }

    diff->search_copy = malloc(sizeof(ListCopy));
    if(diff->search_copy == NULL){
        fprintf(stderr, "Failed to allocate data for search_copy.\n");
        return;
    }
    diff->search_copy->track = NULL;
    diff->search_copy->next = NULL;
    diff->search_copy->prev = NULL;

    if (diff->service2 == YOUTUBE) {
        search_copy_list_youtube(&diff->search_copy, (YoutubePlaylist*)diff->playlist2);
    }
    else{
        search_copy_list_spotify(&diff->search_copy, (SpotifyPlaylist*)diff->playlist2);
    }
}


void diff_list_add_lists_to_diff(DiffList*diff_list, void* list1,Service s1, void* list2, Service s2)
{
    diff_list->service1 = s1;
    diff_list->service2 = s2;

    if (s1 == YOUTUBE) {
        diff_list->playlist1 = (YoutubePlaylist*)list1;
    }
    else {
        diff_list->playlist1 = (SpotifyPlaylist*)list1;
    }

    if (s2 == YOUTUBE) {
        diff_list->playlist2 = (YoutubePlaylist*)list2;
    }
    else {
        diff_list->playlist2 = (SpotifyPlaylist*)list2;
    }
}

void diff_list_init(DiffList** diff_list)
{
    if (diff_list != NULL) {
        fprintf(stderr, "Attempted to initialize diff list that has already been initialized.\n");
        return;
    }
    DiffList* new = malloc(sizeof(DiffList));
    if (new == NULL) {
        fprintf(stderr, "Failed to allocate memory for DiffList.\n");
        return;
    }
    new->playlist1= NULL;
    new->playlist2 = NULL;
    
    new->list = malloc(sizeof(TracksList));
    if (new->list == NULL) {
        free(new);
        return;
    }
    new->list->prev = NULL;
    new->list->next = NULL;
    new->list->tracks = NULL;
    
    *diff_list = new;
    return;
}


void diff_list_free(DiffList** diff_list)
{
    if (*diff_list == NULL) {
        fprintf(stderr, "Attempted to free empty DiffList.\n");
        return;
    }
    
    if ((*diff_list)->list != NULL) {
        while ((*diff_list)->list->next != NULL) {
            (*diff_list)->list = (*diff_list)->list->next;
            free((*diff_list)->list->prev->tracks);
            free((*diff_list)->list->prev);
        }
        free((*diff_list)->list->tracks);
        free((*diff_list)->list);
    }
    free(*diff_list);
    *diff_list = NULL;
}


static void search_copy_list_youtube(ListCopy** list_copy, YoutubePlaylist* playlist)
{
    
    YoutubeTrackList* track_list = playlist->track_list_head;
    ListCopy* new = malloc(sizeof(ListCopy));
    if (new == NULL){
        fprintf(stderr, "Failed to allocate memory for list_copy head.\n");
        return;
    }
    
    new->track = track_list->track;
    new->prev = track_list->prev;
    new->next = track_list->next;
    track_list = track_list->next;
    *list_copy = new;

    while (track_list->track != NULL) {
        ListCopy* new = malloc(sizeof(ListCopy));
        if (new == NULL){
            fprintf(stderr, "Failed to allocate memory for list_copy node.\n");
            return;
        }
        new->track = track_list->track;
        new->prev = track_list->prev;
        new->next = track_list->next;
        track_list = track_list->next;
    }

}


static void search_copy_list_spotify(ListCopy** list_copy, SpotifyPlaylist* playlist){
     
    ListSpotifyTracks* track_list = playlist->track_list;
    ListCopy* new = malloc(sizeof(ListCopy));
    if (new == NULL){
        fprintf(stderr, "Failed to allocate memory for list_copy head.\n");
        return;
    }
    
    new->track = track_list->track;
    new->prev = track_list->prev;
    new->next = track_list->next;
    track_list = track_list->next;
    *list_copy = new;

    while (track_list->track != NULL) {
        ListCopy* new = malloc(sizeof(ListCopy));
        if (new == NULL){
            fprintf(stderr, "Failed to allocate memory for list_copy node.\n");
            return;
        }
        new->track = track_list->track;
        new->prev = track_list->prev;
        new->next = track_list->next;
        track_list = track_list->next;
    }
}


void diff_list_look_for_match(DiffList* diff)
{
    if(diff == NULL) return;

    while (diff->list->tracks != NULL) {
    }

}


