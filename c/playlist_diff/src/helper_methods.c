#include "../header_files/helper_methods.h"

size_t write_callback (void *ptr, size_t size, size_t nmemb, ResponseBuffer *res_buf)
{
    size_t total_size = size * nmemb;

    // Reallocate memory to fit the new data
    char *temp = realloc (res_buf->data, res_buf->size + total_size + 1);
    if (temp == NULL)
    {
        fprintf (stderr, "Failed to allocate memory.\n");
        return 0;		// Returning 0 will signal libcurl to abort the request
    }

    res_buf->data = temp;
    memcpy (&(res_buf->data[res_buf->size]), ptr, total_size);
    res_buf->size += total_size;
    res_buf->data[res_buf->size] = '\0';	// Null-terminate the string

    return total_size;
}


void removeNewline (char *str)
{
    size_t len = strlen (str);
    if (len > 0 && str[len - 1] == '\n') 
    {
        str[len - 1] = '\0';
    }
}

int string_compare_64b (const char *str1, const char *str2)
{
    size_t len = strlen (str1);
    size_t len2 = strlen (str2);

    if (len != len2)
    {
        return 0;
    }
    size_t i;

    // Compare 8 bytes (64 bits) at a time as long as there are 64 or more bits left of the string.
    for (i = 0; i + sizeof (uint64_t) <= len; i += sizeof (uint64_t))
    {
        uint64_t *ptr1 = (uint64_t *) (str1 + i);
        uint64_t *ptr2 = (uint64_t *) (str2 + i);
        if (*ptr1 != *ptr2)
        {
            return 0;
        }
    }
    // Handle remaining bytes (if the string length is not a multiple of 8)
    for (; i < len; ++i)
    {
        if (str1[i] != str2[i])
        {
            return 0;
        }
    }
    return 1;
}


char * curl_request (char *authorization, char *requestURL)
{
    CURLcode ret;
    CURL *hnd;
    ResponseBuffer res_buf;
    struct curl_slist *slist1;
    res_buf.data = malloc (1);
    res_buf.size = 0;

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
    free (requestURL);
    free (authorization);

    hnd = NULL;
    slist1 = NULL;
    requestURL = NULL;
    authorization = NULL;

    return res_buf.data;
}