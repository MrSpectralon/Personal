#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"
#include "../header_files/youtube_methods.h"
#include "../header_files/youtube_playlist.h"
#include <curl/curl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declaration for temporary functionality.
OauthAccess *request_access_from_file(FILE **file_pointer);

static OauthAccess *spotify_credentials_from_file(FILE **file_ptr);
static OauthAccess *ytm_credentials_from_file(FILE **file_ptr);

void test();

int main(void) {
  FILE *fptr = NULL;
  OauthAccess *google_access = NULL;
  SpotifyPlaylist *spotify_playlist = NULL;
  OauthAccess *spotify_access = NULL;
  YoutubePlaylist *youtube_playlist = NULL;

  // of being cleared in memory.
  char *temp = NULL;

  // test();
  // goto cleanup;

  fptr = fopen("hidden_from_git/secrets", "r");
  fprintf(stderr, "File opened.\n");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    goto cleanup;
  }

  spotify_access = request_access_from_file(&fptr);
  if (spotify_access == NULL) {
    fprintf(stderr, "Failed to gain auth key.\n");
    goto cleanup;
  }
  oauth_access_print(spotify_access);

  google_access = request_access_from_file(&fptr);
  if (google_access == NULL) {
    fprintf(stderr, "Failed to gain auth key.\n");
    goto cleanup;
  }
  oauth_access_print(google_access);
  temp = NULL;
  fclose(fptr);

  youtube_playlist = get_youtube_playlist(google_access);

  spotify_playlist = get_spotify_playlist(spotify_access);
  print_spotify_playlist(spotify_playlist);
  yt_playlist_print(youtube_playlist);

cleanup:

  free((char *)temp);
  oauth_access_delete(&google_access);
  oauth_access_delete(&spotify_access);
  spotify_free_playlist(&spotify_playlist);
  yt_playlist_free(&youtube_playlist);
  return 0;
}

OauthAccess *request_access_from_file(FILE **file_pointer) {
  OauthAccess *access;
  const uint64_t BUFFER_LEN = 100;
  char *service_str = NULL;

  readline(file_pointer, &service_str);
  if (service_str == NULL) {
    fprintf(stderr, "Service string failed to allocate.\n");
    goto cleanup;
  }

  if (string_compare_64b(service_str, "spotify")) {
    access = spotify_credentials_from_file(file_pointer);
  } else {
    access = ytm_credentials_from_file(file_pointer);
    // auth_reply = get_auth_token_google((const char*)clientID, (const
    // char*)clientSecret);
  }
  free(service_str);
  service_str = NULL;
  return access;

cleanup:

  free(service_str);
  oauth_access_delete(&access);
  return NULL;
}

static OauthAccess *spotify_credentials_from_file(FILE **file_ptr) {
  char *clientID = NULL;
  char *clientSecret = NULL;
  char *playlist_id = NULL;
  char *auth_reply = NULL;
  OauthAccess *access = NULL;

  size_t client_id_len = readline(file_ptr, &clientID);
  if (clientID == NULL) {
    fprintf(stderr, "Failed to allocate memory for clientID.\n");
    goto cleanup;
  }

  size_t secret_len = readline(file_ptr, &clientSecret);
  if (clientSecret == NULL) {
    fprintf(stderr, "Failed to allocate memory for clientSecret.\n");
    goto cleanup;
  }

  readline(file_ptr, &playlist_id);
  if (playlist_id == NULL) {
    goto cleanup;
  }

  auth_reply = get_auth_token_spotify(clientID, clientSecret);
  if (auth_reply == NULL) {
    fprintf(stderr, "Failed to get auth reply.\n");
    goto cleanup;
  }

  explicit_bzero(clientID, client_id_len);
  explicit_bzero(clientSecret, secret_len);
  free(clientID);
  free(clientSecret);

  clientID = NULL;
  clientSecret = NULL;
  access = oauth_access_init(auth_reply, SPOTIFY, playlist_id);
  if (access == NULL) {
    fprintf(stderr, "Failed to init access object.\n");
    goto cleanup;
  }
  explicit_bzero(auth_reply, strlen(auth_reply));
  free(auth_reply);
  auth_reply = NULL;

  free(playlist_id);
  playlist_id = NULL;

  return access;

cleanup:

  if (clientID != NULL) {
    explicit_bzero(clientID, client_id_len);
  }
  free(clientID);

  if (clientSecret != NULL) {
    explicit_bzero(clientSecret, secret_len);
  }
  free(clientSecret);

  return NULL;
}

static OauthAccess *ytm_credentials_from_file(FILE **file_ptr) {

  const int BUFFER_SIZE = 550;
  char *temp = NULL;
  char *clientID = NULL;

  // Shall be redirect url (temporarily™)
  char *clientSecret = NULL;
  char *playlist_id = NULL;
  char *auth_reply = NULL;
  OauthAccess *access = NULL;

  // size_t secret_len = 0;
  // size_t id_len = 0;
  // size_t pl_len = 0;

  size_t id_len = readline(file_ptr, &clientID);
  if (clientID == NULL) {
    goto cleanup;
  }

  size_t secret_len = readline(file_ptr, &clientSecret);
  if (clientSecret == NULL) {
    goto cleanup;
  }

  readline(file_ptr, &playlist_id);
  if (playlist_id == NULL) {
    goto cleanup;
  }

  // TODO: Make webserver to handle this step.
  //  auth_reply = get_auth_token_google(Some parameter.);

  printf("Paste in entire reply json: ");
  size_t auth_len = readline(&stdin, &auth_reply );
  if (auth_reply == NULL)
    goto cleanup;
	explicit_bzero(clientID, id_len);
	explicit_bzero(clientSecret, secret_len);
  free(clientID);
  free(clientSecret);
  clientID = NULL;
  clientSecret = NULL;

  access = oauth_access_init(auth_reply, YOUTUBE, playlist_id);
  if (access == NULL) {
    fprintf(stderr, "Failed to initialize access object");
    goto cleanup;
  }
	explicit_bzero(auth_reply, auth_len);
  free(auth_reply);
  auth_reply = NULL;

  free(playlist_id);
  playlist_id = NULL;

  return access;

cleanup:
  if (clientID != NULL) {
    memset((char *)clientID, 0, strlen((char *)clientID));
  }
  if (temp != NULL) {
    memset((char *)temp, 0, BUFFER_SIZE);
  }
  free((char *)temp);
  free((char *)clientSecret);
  free(auth_reply);
  free(playlist_id);

  return NULL;
}

char *get_usr_input(char *print_msg) {
  char buffer[600];
  printf("%s", print_msg);
  scanf("%s", buffer);
  size_t s_len = strlen(buffer);
  char *temp = calloc(s_len + 1, sizeof(char));
  if (temp == NULL) {
    return NULL;
  }
  strcpy(temp, buffer);

  return temp;
}

void test() {

  char *play_name = NULL;
  char *play_desc = NULL;
  char *play_id = NULL;

  char *name = NULL;
  char *description = NULL;
  char *duration = NULL;
  char *artist = NULL;
  char *id = NULL;

  YoutubePlaylist *ytp = NULL;

  char tmp[2];
  ytp = malloc(sizeof(YoutubePlaylist));
  if (ytp == NULL) {
    goto end;
  }
  yt_playlist_init(&ytp);

  printf("P - Set playlist data.\n");
  printf("T - Add track data.\n");
  printf("L - List all added tracks.\n");
  printf("D - Delete all tracks.\n");
  printf("Q - Quit.\n");
  while (1) {
    printf("OPTION: ");
    scanf("%s", tmp);

    switch (tmp[0]) {
    case 'P':
      free(play_name);
      free(play_desc);
      free(play_id);

      printf("Playlist:\n");
      play_name = get_usr_input("\tName: ");
      if (play_name == NULL)
        goto end;
      play_desc = get_usr_input("\tDescription: ");
      if (play_desc == NULL)
        goto end;
      play_id = get_usr_input("\tID: ");
      if (play_id == NULL)
        goto end;

      free(ytp->name);
      free(ytp->description);
      free(ytp->id);
      ytp->name = strdup(play_name);
      if (ytp->name == NULL)
        goto end;
      ytp->description = strdup(play_desc);
      if (ytp->description == NULL)
        goto end;
      ytp->id = strdup(play_id);
      if (ytp->id == NULL)
        goto end;

      break;

    case 'T':
      free(name);
      free(description);
      free(duration);
      free(artist);
      free(id);
      printf("Song:\n");
      name = get_usr_input("\tName: ");
      if (name == NULL)
        goto end;
      artist = get_usr_input("\tArtist: ");
      if (artist == NULL)
        goto end;
      description = get_usr_input("\tDescription: ");
      if (description == NULL)
        goto end;
      duration = get_usr_input("\tDuration: ");
      if (duration == NULL)
        goto end;
      id = get_usr_input("\tID: ");
      if (id == NULL)
        goto end;

      YtTrack *new_track =
          yt_track_create(id, name, artist, description, duration);
      if (new_track == NULL) {
        goto end;
      }
      yt_track_list_append(&ytp->track_list_head, new_track);
      break;

    case 'L':
      yt_playlist_print(ytp);
      break;

    case 'D':
      yt_track_list_free(&(ytp->track_list_head));
      break;

    case 'H':
      printf("P - Set playlist data.\n");
      printf("T - Add track data.\n");
      printf("L - List all added tracks.\n");
      printf("D - Delete all tracks.\n");
      printf("Q - Quit.\n");
      break;
    case 'Q':
      goto end;
    default:
      break;
    }
  }

end:

  free(play_name);
  free(play_desc);
  free(play_id);

  free(name);
  free(description);
  free(duration);
  free(artist);
  free(id);
  yt_playlist_free(&ytp);
}
