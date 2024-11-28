#pragma once
#include <stdlib.h>

typedef struct {
    char* id;
    char* title;
    char* artist;
    //Skipping these members, as i will have to go a seperate API call for each track/video
    //And each call costs one unit of query quota.
    // char* description;
    // char* duration; // ISO 8601

} YtTrack;

typedef struct YoutubeTrackList{
    YtTrack* track;

    struct YoutubeTrackList* next;
    struct YoutubeTrackList* prev;

} YoutubeTrackList;

typedef struct {
    char* name;
    char* description;
    char* id;
    int total_tracks;
    YoutubeTrackList* track_list_head;
    
} YoutubePlaylist;

YtTrack* yt_track_create(char* id, char* title, char* artist, char* description, char* duration_ms);
void yt_track_print(YtTrack* track);
void yt_track_free(YtTrack** track);

int yt_track_list_append(YoutubeTrackList** track_list, YtTrack* track);
void yt_track_list_print(YoutubeTrackList* track_list);
void yt_track_list_free(YoutubeTrackList** track_list);

void yt_playlist_init(YoutubePlaylist** playlist);
void yt_playlist_print(YoutubePlaylist* playlist);
void yt_playlist_free(YoutubePlaylist** playlist);


