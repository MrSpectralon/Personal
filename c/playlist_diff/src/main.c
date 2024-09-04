#include "../header_files/helper_methods.h"
#include "../header_files/spotify_methods.h"

FILE *fptr;
int main(void)
{
  char* clientID;
  char* clientSecret;
  char* temp;

  fptr = fopen("secrets", "r");
  temp = malloc(100);

  fgets(temp, 100, fptr);
  clientID = malloc(strlen(temp)); 
  removeNewline(temp);
  strcpy(clientID, temp);

  fgets(temp, 100, fptr);
  clientSecret = malloc(strlen(temp));
  removeNewline(temp);
  strcpy(clientSecret, temp);
  fclose(fptr);
  
  char* auth_reply = getAuthTokenSpotify(clientID, clientSecret);
  printf("Spotify reply:\n%s\n", auth_reply);
  SpotifyAccess *access; 
  
  access = spotify_access_init(auth_reply);
  print_spotify_access(access);
  cleanup:

  memset(temp, '\0', sizeof(temp));
  memset(clientID, '\0', sizeof(clientID));
  memset(clientSecret, '\0', sizeof(clientSecret));
  memset(auth_reply, '\0', sizeof(auth_reply));

  free(temp);
  free(clientID);
  free(clientSecret);
  free(auth_reply);
  
  spotify_access_delete(access);

  return 0;
}
