#include <stdio.h>
#include <curl/curl.h> //Install: apt install libcurl4-openssl-dev


int main(){
	printf("Nu kjorar me!");
	CURL *curl = curl_easy_init();
	if(curl) {
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, "");
	}
	
	return 0;
}
