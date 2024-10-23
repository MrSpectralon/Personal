#include "../header_files/curl_handler.h"


int response_buffer_init(ResponseBuffer* rb)
{
    rb->data = malloc (1);
    if (rb->data == NULL)
    {
        fprintf(stderr, "Malloc failed to allocate memory for response buffer data.\n");
        return 0;
    }
    rb->size = 0;
    return 1;
}

size_t write_callback (void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf)
{
    size_t total_size = size * nmemb;

    // Reallocate memory to fit the new data
    char *temp = realloc (res_buf->data, res_buf->size + total_size + 1);
    if (temp == NULL)
    {
        fprintf (stderr, "Failed to reallocate memory in write_callback.\n");
        return 0; // Returning 0 will signal libcurl to abort the request
    }

    res_buf->data = temp;
    memcpy (&(res_buf->data[res_buf->size]), ptr, total_size);
    res_buf->size += total_size;
    res_buf->data[res_buf->size] = '\0';	// Null-terminate the string

    return total_size;
}

char* curl_get_request (const char* authorization, const char* requestURL)
{
    CURLcode ret;
    CURL *hnd;
    ResponseBuffer res_buf;
    
     
    if (!response_buffer_init(&res_buf))
    {
        return NULL;
    }

    struct curl_slist *slist1;
    slist1 = NULL;
    slist1 = curl_slist_append (slist1, authorization);
    hnd = curl_easy_init ();

    curl_easy_setopt (hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt (hnd, CURLOPT_URL, requestURL);
    curl_easy_setopt (hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt (hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt (hnd, CURLOPT_USERAGENT, "curl/8.5.0");
    curl_easy_setopt (hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt (hnd, CURLOPT_HTTP_VERSION,
		      (long) CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt (hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt (hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt (hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, &res_buf);

    ret = curl_easy_perform (hnd);

    if (ret != CURLE_OK)
      {
	  fprintf (stderr, "curl_easy_perform() failed: %s\n",
		   curl_easy_strerror (ret));
      }

    curl_easy_cleanup (hnd);
    curl_slist_free_all (slist1);
    hnd = NULL;
    slist1 = NULL;
    return res_buf.data;
}

char* curl_post_request(

    const char* destination_url,
    const char* content_type,
    const char* post_field, 
    const long request_len)

{
    
    //Initialize buffer struct.
    ResponseBuffer res_buf;
    if (!response_buffer_init(&res_buf))
    {
       return NULL; 
    }
    
    CURLcode ret;
    CURL *hnd;

    struct curl_slist *slist1;
    slist1 = NULL;
    slist1 = curl_slist_append (slist1, content_type);
    hnd = curl_easy_init ();

    curl_easy_setopt (hnd, CURLOPT_BUFFERSIZE, 102400L);
    curl_easy_setopt (hnd, CURLOPT_URL, destination_url);
    curl_easy_setopt (hnd, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt (hnd, CURLOPT_POSTFIELDS, post_field);
    curl_easy_setopt (hnd, CURLOPT_POSTFIELDSIZE_LARGE,
              (curl_off_t) request_len);
    curl_easy_setopt (hnd, CURLOPT_HTTPHEADER, slist1);
    curl_easy_setopt (hnd, CURLOPT_USERAGENT, "curl/8.5.0");
    curl_easy_setopt (hnd, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt (hnd, CURLOPT_HTTP_VERSION,
              (long) CURL_HTTP_VERSION_2TLS);
    curl_easy_setopt (hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt (hnd, CURLOPT_FTP_SKIP_PASV_IP, 1L);
    curl_easy_setopt (hnd, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt (hnd, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt (hnd, CURLOPT_WRITEDATA, &res_buf);

    ret = curl_easy_perform (hnd);

    if (ret != CURLE_OK)
    {
        fprintf (stderr, "curl_easy_perform() failed: %s\n",
        curl_easy_strerror (ret));
    }

    curl_easy_cleanup (hnd);
    hnd = NULL;
    curl_slist_free_all (slist1);
    slist1 = NULL;
    return res_buf.data;
}

