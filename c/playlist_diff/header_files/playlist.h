//List structure for Spotify and youtube music services.
#pragma once

typedef enum
{
	NONE,
	SPOTIFY,
	YOUTUBE
} Service;

const char* SERVICE_STR[] = {"None/NA", "Spotify", "YouTube-Music"};


typedef struct
{
	char* id;
	char* name;
	char* album;
	char* artist;
	// Spotify uses int and youtube uses strings for durations.
	void* duration;
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
	char* details;
	int total_tracks;
	TrackList* tracks;
} Playlist;

Track* track_init();
void track_free(Track** track);


void tracklist_free(TrackList** tracklist);

Playlist* playlist_init();
void playlist_free(Playlist** playlist);

void playlist_append(Track* track, TrackList** head);
void playlist_pop(TrackList** node);

char* playlist_get_track_duration(Track* track, Service service);
