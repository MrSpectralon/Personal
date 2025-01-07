#pragma once

#include "./oauth_handler.h"
#include "./spotify_track_list.h"
#include "./youtube_playlist.h"


typedef enum {
	NIL,			// NULL - needed as polaceholder during copy phase. Indication that no evaluation of the track has been made.
	MATCH,			// In both playlists
	PARTIAL_MATCH,	// Potentially in both playlists
	NOT_FOUND,		// Track only found in one playlist
	ADDING			// Will add to the other playlist
	
} Status;


typedef struct{
	void* track1;
	void* track2;
	Status status;
} Tracks;


typedef struct TracksList{
	Tracks* tracks;
	
	struct TracksList* next;
	struct TracksList* prev;

} TracksList;

typedef struct {
	void* track;
	void* next;
	void* prev;
} ListCopy;


typedef struct {
	
	Service service1;
	void* playlist1;
	
	Service service2;
	void* playlist2;
	
	ListCopy* search_copy;

	TracksList* list;
} DiffList;



int diff_tracks_list_append_spotify(TracksList** diff_list_head, ListSpotifyTracks* list_node);
int diff_tracks_list_append_youtube(TracksList** diff_list_head, YoutubeTrackList* list_node);
void diff_list_append_playlist1(DiffList* diff);
void diff_list_add_lists_to_diff(DiffList* diff_list, void* list1,Service s1, void* list2, Service s2);
void diff_list_init(DiffList** diff_list);
void diff_list_free(DiffList** diff_list);


