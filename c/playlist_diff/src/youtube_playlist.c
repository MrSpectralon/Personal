#include "../header_files/youtube_playlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static int allocate_and_populate(char** dest, char* src);
static char* duration_printable(char* duration);

static void duration_pr_inner_loop(char** pd, size_t* pdi, char* str, size_t start_index, size_t stop_index);


static int allocate_and_populate(char** dest, char* src)
{
    *dest = (char*)malloc(strlen(src) + 1);
    if (dest == NULL) {
        fprintf(stderr, "\nError occured when allocating memory for youtube playlist.\n");
        return 0;
    }
    *dest[0] = '\0';
    strcpy(*dest, src);
    return 1;
}

//pd = printable date
static void duration_pr_inner_loop(char** pd, size_t* pdi, char* str, size_t start_index, size_t stop_index)
{
    for (size_t i = start_index; i < stop_index; i++) {
        (*pd)[(*pdi)] = str[i];
        (*pdi)++;
    }
}

static char* duration_printable(char* duration)
{
    size_t d_indx = 0;
    size_t indx_tmp = 0;
    
    size_t len = strlen(duration);
    char* d = malloc(len);
    if (d == NULL) 
    {
        fprintf(stderr, "\nFailed to allocate memory for printable track duration\n");
        return NULL;
    }
    
    indx_tmp = (duration[1] == 'T') ? 2 : 1;

    for (size_t i = indx_tmp; i < len; i++) {
        switch (duration[i]) {
            case 'D':
                //TODO: Test with videos that are more than 24 hours long. 
                // Probably won't work.
                duration_pr_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                i+=2;
                indx_tmp = i;
                d[d_indx] = ':';
                d_indx++;
                break;
            case 'H':
            case 'M':
                duration_pr_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                i++;
                indx_tmp = i;
                d[d_indx] = ':';
                d_indx++;
                break;
            case 'S':
                duration_pr_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                d[d_indx] = '\0';
                break;
            default:
                break;
        };
    }
    return d;
}

YtTrack* yt_track_create(char* id, char* title, char* artist, char* description, char* duration)
{
    YtTrack* track = NULL;
    track = malloc(sizeof(YtTrack));
    if (track == NULL) {
        goto cleanup;
    }
    track->id = NULL;
    track->title = NULL;
    track->artist = NULL;
    // track->description = NULL;
    // track->duration = NULL;

    if (!allocate_and_populate(&track->id, id)) goto cleanup;
    if (!allocate_and_populate(&track->title, title)) goto cleanup;
    if (!allocate_and_populate(&track->artist, artist)) goto cleanup;
    // if (!allocate_and_populate(&track->description, description)) goto cleanup;
    // if (!allocate_and_populate(&track->duration, duration)) goto cleanup;
    
    return track;

cleanup:
    if (track != NULL) 
    {
        free(track->id);
        free(track->title);
        free(track->artist);
        // free(track->duration);
        // free(track->description);
    }
    free(track);
    return NULL;
}


int yt_track_list_append(YoutubeTrackList** track_list, YtTrack* track)
{
    if ((*track_list)->track == NULL) {
        (*track_list)->track = track;
        return 0;
    }

    YoutubeTrackList* new_entry = malloc(sizeof(YoutubeTrackList));
    if (new_entry == NULL) {
        fprintf(stderr, "\nFailed to allocate memory for new youtube list entry.\n");
        return 1;
    }
    new_entry->track = track;
    
    if ((*track_list)->next == NULL || (*track_list)->prev == NULL) {
        new_entry->next = (*track_list);
        (*track_list)->prev = new_entry;
        new_entry->prev = NULL; 
        *track_list = new_entry;
        return 0;
    }
    
    new_entry->next = (*track_list)->next;
    new_entry->next->prev = new_entry;

    new_entry->prev = (*track_list)->prev;
    new_entry->prev->next = new_entry;
    return 0;
}


void yt_playlist_init(YoutubePlaylist** playlist)
{
    *playlist = malloc(sizeof(YoutubePlaylist));
    if (playlist == NULL) {
        fprintf(stderr, "Failed to allocate YoutubePlaylist\n");
        return;
    }
    (*playlist)->name = NULL;
    (*playlist)->description = NULL;
    (*playlist)->id = NULL;
    (*playlist)->total_tracks = 0;
    (*playlist)->track_list_head = malloc(sizeof(YoutubeTrackList));
    if ((*playlist)->track_list_head == NULL) {
        fprintf(stderr, "Failed to initialize track list in playlist.");
        return;
    }
    (*playlist)->track_list_head->next = NULL;
    (*playlist)->track_list_head->prev = NULL;
    (*playlist)->track_list_head->track = NULL;
}

void yt_track_print(YtTrack* track)
{
    // char* duration = NULL;
    // duration = duration_printable(track->duration);
    // if (duration == NULL) {
    //     return;
    // }
    printf("\n\tName: %s\n", track->title);
    printf("\tAccount: %s\n", track->artist);
    // printf("\tDescription: \n\t\t%s\n", track->description);
    // printf("\tDuration: %s\n", duration);
    printf("\tURL: https://www.youtube.com/watch?v=%s\n", track->id);
    // free(duration);
}

void yt_track_list_print(YoutubeTrackList* track_list)
{
    if (track_list == NULL) return;

    if (track_list->track == NULL ) {
        fprintf(stderr, "Attempted to print empty YT track list.\n"); 
        return;
    }

    do { 
        yt_track_print(track_list->track);
        printf("\n");
        track_list = track_list->next;
    } while (track_list != NULL);
}


void yt_playlist_print(YoutubePlaylist *playlist)
{
    printf("### YouTube Playlist ###\n");
    printf("Name: %s\n", playlist->name);
    printf("Description: %s\n", playlist->description);
    printf("URL: https://youtube.com/playlist?list=%s\n", playlist->id);
    yt_track_list_print(playlist->track_list_head);
}


void yt_track_free(YtTrack** track)
{
    if (*track == NULL) {
        return;
    }
    free((*track)->id);
    free((*track)->title);
    free((*track)->artist);
    // free((*track)->duration);
    // free((*track)->description);
    free((*track));
    *track = NULL;
}


void yt_track_list_free(YoutubeTrackList** track_list)
{
    YoutubeTrackList* tmp = NULL;
    if (*track_list == NULL) {
        return;
    }
    if ((*track_list)->prev != NULL) {
        fprintf(stderr, "Attempted to delete youtube track list from another point than head.");
        return;
    }
    while ((*track_list)->next != NULL)
    {
        tmp = (*track_list)->next;
        (*track_list)->next = tmp->next;
        yt_track_free(&tmp->track);
        free(tmp);
        
    }
    yt_track_free(&(*track_list)->track);
    free(*track_list);
    *track_list = NULL;
}


void yt_playlist_free(YoutubePlaylist** playlist)
{
    if (*playlist == NULL) {
        return;
    }
    yt_track_list_free(&(*playlist)->track_list_head);
    free((*playlist)->name);
    free((*playlist)->description);
    free((*playlist)->id);
    free(*playlist);
    *playlist = NULL;
}

