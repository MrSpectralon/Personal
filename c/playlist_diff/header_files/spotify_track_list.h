#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    char* name;
    int duration_ms;
    char *album;
    char *artist;
    char *preview_url;
    char *external_url; 
} SpotifyTrack;

typedef struct ListSpotifyTracks{

    SpotifyTrack* track;

    struct ListSpotifyTracks* prev;
    struct ListSpotifyTracks* next;

} ListSpotifyTracks;

typedef struct{

    char* name;
    char* description;
    int total_tracks;

    ListSpotifyTracks* track_list;
} SpotifyPlaylist;



int append_spotify_track(ListSpotifyTracks** list_head, const char *name, const char* album, const char* artist, const int duration, const char* preview_url, const char* external_url);

void pop_spotify_track(ListSpotifyTracks* track, ListSpotifyTracks* list_head);
void free_spotify_track(SpotifyTrack** track_data);
void print_list_content(ListSpotifyTracks* list_head);

void spotify_free_tracklist(ListSpotifyTracks** list_head);
void spotify_free_playlist(SpotifyPlaylist** playlist);
SpotifyPlaylist* spotify_playlist_init();

ListSpotifyTracks* find_same_track(ListSpotifyTracks* list_head, const SpotifyTrack* track);

int same_song(const SpotifyTrack* track1, const SpotifyTrack* track2);
