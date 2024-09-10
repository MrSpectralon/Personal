#pragma once

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev
#include <cjson/cJSON.h>


typedef struct {
  char *data;
  size_t size;

} ResponseBuffer;

typedef struct {
  char *token;
  char *type;
  int duration;
  struct tm *time_recieved;
    
} SpotifyAccess;

typedef struct {
  void *next;

  char *track_name;
  char *track_album;
  char *track_artist;
  int track_duration;
  char *track_preview_url;
  char *track_external_url; 

} SpotifyNode;

SpotifyNode *spotify_node_head = NULL;

void free_spotify_list(SpotifyNode *list);
//SpotifyTrackList *add_track(char *name, char *album, char *artist, int duration, char *preview_url, char *external_url);
//void append_track(SpotifyTrackList **head, SpotifyTrackList *new_node);




void get_spotify_playlist( 
  SpotifyAccess *access, 
  char *playlist_id
  );

SpotifyNode *add_spotify_node( 
  char *track_name, 
  char *track_album, 
  char *track_artist, 
  int duration, 
  char *preview_url, 
  char *spotify_url
  );
char* getPlaylistContentSpotify(char* playlistID, SpotifyAccess* ad, int offset);
int parce_spotify_reply_data(char* data);
void print_spotify_list(SpotifyNode **head);



size_t write_callback(void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf);

char* getAuthTokenSpotify(char* clientID, char* clientSecret);


void getAuthKey(CURL *opts, CURLcode res);

SpotifyAccess *spotify_access_init(char* auth_reply);
void print_spotify_access(SpotifyAccess *access_obj); 
void spotify_access_delete(SpotifyAccess *access_obj);