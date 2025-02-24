//List structure for Spotify and youtube music services.
//
#pragma once
#include "./helper_methods.h"


typedef struct
{
	char* id;
	char* name;
	char* album;
	char* artist;
	// Spotify uses int and youtube uses strings for durations.
	char* duration;
	char* url;

} Track;

typedef struct TrackList
{
	Track* track;
	struct TrackList* next;
	struct TrackList* prev;

} TrackList;

typedef struct 
{
	Service service;
	char* id;
	char* name;
	char* description;
	int total_tracks;
	TrackList* tracks;
} Playlist;

Track* track_init();
void track_free(Track** track);
void track_print(Track* track);

void tracklist_free(TrackList** tracklist);
void tracklist_print(TrackList* tracklist_head);

Playlist* playlist_init();
void playlist_free(Playlist** playlist);
void playlist_print_details(Playlist* playlist);
void playlist_print(Playlist* playlist);

void playlist_append(Track* track, TrackList** head);
void tracklist_pop(TrackList** node);

void track_set_duration(Track** track, Service service, void* duration);
