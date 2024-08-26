#include <stdio.h>
#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev
#include <string.h>
#include <stdlib.h>

CURLcode getAuthTokenSpotify(char* clientID, char* clientSecret);
void removeNewline(char* str);
FILE *fptr;


int main(void)
{
  fptr = fopen("secrets", "r");
  char* clientID;
  char* clientSecret;
  char* temp;
  
  temp = malloc(100);

  fgets(temp, 100, fptr);
  clientID = malloc(strlen(temp)); 
  removeNewline(temp);
  strcpy(clientID, temp);

  fgets(temp, 100, fptr);
  clientSecret = malloc(strlen(temp));
  removeNewline(temp);
  strcpy(clientSecret, temp);
  
  free(temp);
  fclose(fptr);

  CURLcode authToken = getAuthTokenSpotify(clientID, clientSecret);
  printf("%d\n", authToken);
  
  free(clientID);
  free(clientSecret);
  return 0;
}

void removeNewline(char* str)
{
  size_t len = strlen(str);
  if (len > 0 && str[len-1] == '\n')
  {
    str[len-1] = '\0';
  };
}


CURLcode getAuthTokenSpotify(char* clientID, char* clientSecret)
{
  CURLcode ret;
  CURL *hnd;
  char* curlPostField; 
  struct curl_slist *slist1;
  
  char grantTypeAndClientID[] = "grant_type=client_credentials&client_id=";
  char cliS[] = "&client_secret=";

  int length = strlen(grantTypeAndClientID);
 
  curlPostField = (char*)malloc(strlen(grantTypeAndClientID)
                         + strlen(clientID)
                         + strlen(cliS)
                         + strlen(clientSecret));
  curlPostField[0] = '\0';
  strcat(curlPostField, grantTypeAndClientID);
  strcat(curlPostField, clientID);
  strcat(curlPostField, cliS);
  strcat(curlPostField, clientSecret);
  int fieldSize = strlen(curlPostField);

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type: application/x-www-form-urlencoded");
  hnd = curl_easy_init();

  curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
  curl_easy_setopt(hnd, CURLOPT_URL, "https://accounts.spotify.com/api/token");
  curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, curlPostField);
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)fieldSize);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/8.5.0");
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  ret = curl_easy_perform(hnd);

  curl_easy_cleanup(hnd);
  hnd = NULL;
  free(curlPostField);
  curl_slist_free_all(slist1);
  slist1 = NULL;
  return (int)ret;
}
