#include "../header_files/playlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* youtube_duration_converstion(char* duration);
static void youtube_duration_inner_loop(char** pd, size_t* pdi, char* str, size_t start_index, size_t stop_index);
static char* spotify_duration_conversion(int* duration);

const char* SERVICE_STR[] = {"None/NA", "Spotify", "YouTube-Music"};

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


void track_free(Track** track)
{
    free((*track)->id);
    printf("Freeing track:\t%s\n", (*track)->name);
    free((*track)->name);
    free((*track)->album);
    free((*track)->artist);
    free((*track)->duration);
    free((*track)->url);
    free((*track));
    *track = NULL;
}

void track_print(Track* track)
{
    printf("\tName: %s\tAlbum: %s\tArtist: %s\tDuration: %s\n", 
           track->name, track->album, track->artist, track->duration);
    printf("\tURL: %s\n", track->url);

}

void tracklist_free(TrackList **tracklist)
{

    while ((*tracklist)->next != NULL) {
        TrackList* tmp = (*tracklist)->next;
        if(tmp == NULL) break;

        track_free(&tmp->track);
        (*tracklist)->next = (tmp->next == NULL) ? NULL : tmp->next;
        free(tmp);
    }
    if ((*tracklist)->track != NULL)
        track_free(&(*tracklist)->track);
    free(*tracklist);
    *tracklist = NULL;
}

void tracklist_print(TrackList* tracklist_head)
{
    while (tracklist_head != NULL) {
        printf("\n");
        track_print(tracklist_head->track);
        tracklist_head = tracklist_head->next;
    }
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
    new->description = NULL;
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

void playlist_free(Playlist **playlist)
{
    tracklist_free(&(*playlist)->tracks);
    free((*playlist)->id);
    free((*playlist)->name);
    free((*playlist)->description);
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
    if (node == NULL || *node == NULL) return;

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

void track_set_duration(Track** track, Service service, void* duration)
{
    if (service == NONE) return;
    if (duration == NULL) return;
    if (service == SPOTIFY) {
        (*track)->duration = spotify_duration_conversion((int*)duration);
        return;
    }
    (*track)->duration = youtube_duration_converstion((char*)duration);
}


static char* spotify_duration_conversion(int* duration)
{
    int len = 0;
    char buffer[10] = {};
    
    char* duration_str = malloc(1);
    if (duration_str == NULL) {
        fprintf(stderr, "Failed to allocate memory for string duration.");
        return NULL;
    }
    duration_str[0] = '\0';

    int hours = *duration / (60*60*1000);
    int hours_ms = hours * 60 * 60 * 1000;
    int minutes = (*duration - hours_ms) / (60*1000);
    int minutes_ms = minutes * 60 * 1000;
    int seconds = (*duration - (hours_ms + minutes_ms)) / 1000;
    int ms = (*duration - (hours_ms + minutes_ms + (seconds*1000))) / 100;
    if (hours > 0) {
        sprintf(buffer, "%d", hours);
        len += strlen(buffer) + 2; // +1 because of : and + 1 because of \0
        duration_str = realloc(duration_str, len);
        if (duration_str == NULL) {
            fprintf(stderr, "Failed to realloc duration_str.\n");
            return NULL;
        }
        strcat(duration_str, buffer);
        duration_str[len-1] = ':';
        memset(buffer, '\0', 10);
    }
    if (minutes > 0 || hours > 0) {
        sprintf(buffer, "%d", minutes);
        len += strlen(buffer) + 2; 
        duration_str = realloc(duration_str, len);
        if (duration_str == NULL) {
            fprintf(stderr, "Failed to realloc duration_str.\n");
            return NULL;
        }
        strcat(duration_str, buffer);
        duration_str[len-1] = ':';
        memset(buffer, '\0', 10);
    }

    sprintf(buffer, "%d", seconds);
    len += strlen(buffer) + 2; 
    duration_str = realloc(duration_str, len);
    if (duration_str == NULL) {
        fprintf(stderr, "Failed to realloc duration_str.\n");
        return NULL;
    }
    strcat(duration_str, buffer);
    duration_str[len-1] = ',';
    memset(buffer, '\0', 10);

    sprintf(buffer, "%d", ms);
    len += strlen(buffer) + 1; 
    duration_str = realloc(duration_str, len);
    if (duration_str == NULL) {
        fprintf(stderr, "Failed to realloc duration_str.\n");
        return NULL;
    }
    strcat(duration_str, buffer);

    return duration_str;
}


//pd = printable duration
static void youtube_duration_inner_loop(char** pd, size_t* pdi, char* str, size_t start_index, size_t stop_index)
{
    for (size_t i = start_index; i < stop_index; i++) {
        (*pd)[(*pdi)] = str[i];
        (*pdi)++;
    }
}

static char* youtube_duration_converstion(char* duration)
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
                youtube_duration_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                i+=2;
                indx_tmp = i;
                d[d_indx] = ':';
                d_indx++;
                break;
            case 'H':
            case 'M':
                youtube_duration_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                i++;
                indx_tmp = i;
                d[d_indx] = ':';
                d_indx++;
                break;
            case 'S':
                youtube_duration_inner_loop(&d, &d_indx, duration, indx_tmp, i);
                d[d_indx] = '\0';
                break;
            default:
                break;
        };
    }
    return d;
}


void playlist_print_details(Playlist* playlist)
{
    printf("Name: %s\tService: %s\tID: %s\n", playlist->name, SERVICE_STR[playlist->service], playlist->id);
    printf("Description: %s\n", playlist->description);
    printf("Total tracks: %d\n", playlist->total_tracks);
}

void playlist_print(Playlist *playlist)
{
    playlist_print_details(playlist);
    tracklist_print(playlist->tracks);
}
