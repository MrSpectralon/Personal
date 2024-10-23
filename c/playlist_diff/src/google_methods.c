#include "../header_files/google_methods.h"



cJSON* create_JWT()
{
    return NULL;
    //cJSON* jwt = cJSON_CreateObject();
    

}




char* get_auth_token_google (const char *clientID, const char *clientSecret)
{
    return NULL;
    // ResponseBuffer res_buf;
    // res_buf.data = malloc (1);
    // res_buf.size = 0;

    // CURLcode ret;
    // CURL *hnd;
    // char *curlPostField;
    // struct curl_slist *slist1;

    // char grantTypeAndClientID[] = "grant_type=client_credentials&client_id=";
    // char cliS[] = "&client_secret=";

    // int fieldSize = 
	// 	strlen (grantTypeAndClientID)
	// 	+ strlen (clientID) 
	// 	+ strlen (cliS) 
	// 	+ strlen (clientSecret);

    // curlPostField = calloc (fieldSize, sizeof (char));
    // strcat (curlPostField, grantTypeAndClientID);
    // strcat (curlPostField, clientID);
    // strcat (curlPostField, cliS);
    // strcat (curlPostField, clientSecret);

    // slist1 = NULL;
    // slist1 =
	// curl_slist_append (slist1,
	// 		   "Content-Type: application/x-www-form-urlencoded");
    // hnd = curl_easy_init ();

    // curl_easy_setopt (hnd, CURLOPT_BUFFERSIZE, 102400L);
    // curl_easy_setopt (hnd, CURLOPT_URL,
	// 	      "https://accounts.google.com/o/oauth2/v2/auth");
    // curl_easy_setopt (hnd, CURLOPT_NOPROGRESS, 1L);
    // curl_easy_setopt (hnd, CURLOPT_POSTFIELDS, curlPostField);
    // curl_easy_setopt (hnd, CURLOPT_POSTFIELDSIZE_LARGE,
	// 	      (curl_off_t) fieldSize);
    // curl_easy_setopt (hnd, CURLOPT_HTTPHEADER, slist1);
    // curl_easy_setopt (hnd, CURLOPT_USERAGENT, "curl/8.5.0");
    // curl_easy_setopt (hnd, CURLOPT_MAXREDIRS, 50L);
    // curl_easy_setopt (hnd, CURLOPT_HTTP_VERSION,
	// 	      (long) CURL_HTTP_VERSION_2TLS);
    // curl_easy_setopt (hnd, CURLOPT_CUSTOMREQUEST, "POST");
    // curl_easy_setopt (hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    // curl_easy_setopt (hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    // curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, write_callback);
    // curl_easy_setopt (hnd, CURLOPT_WRITEDATA, &res_buf);

    // ret = curl_easy_perform (hnd);

    // if (ret != CURLE_OK)
	// {
	// 	fprintf (stderr, "curl_easy_perform() failed: %s\n",
	// 	curl_easy_strerror (ret));
	// }

    // curl_easy_cleanup (hnd);
    // hnd = NULL;
    // free (curlPostField);
    // curl_slist_free_all (slist1);
    // slist1 = NULL;
    // return res_buf.data;
}

