/*
*
* This code is under a gpl-3.0 license; see LICENSE.txt for more information.
*
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libnotify/notify.h>
#include <unistd.h>
#include <curl/curl.h>

// code by https://stackoverflow.com/users/1003575/mohamed
// from https://stackoverflow.com/questions/15571922/putting-curl-result-in-a-string-and-not-stdout

int http_get_response(void *buffer, size_t size, size_t rxed, char **msg_in){
  char *c;

  if (asprintf(&c, "%s%.*s", *msg_in, size * rxed, buffer) == -1) {
    free(*msg_in);
    msg_in = NULL;
    return -1;
  }

  free(*msg_in);
  *msg_in = c;

  return size * rxed;
}

int main(int argc, char *argv[]){

  (void) argc;
  (void) argv;

  CURL *curl;
  CURLcode res;

  int is_cascade = 0;

  while (1){

    char *msg_in = calloc(1,sizeof(char));

    curl = curl_easy_init();
    if(curl) {
      curl_easy_setopt(curl, CURLOPT_URL, "https://api.tenno.tools/worldstate/pc/fissures");
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_get_response);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &msg_in);

      res = curl_easy_perform(curl);
      if(res != CURLE_OK)
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));

      curl_easy_cleanup(curl);
    }

    // printf("curl: %s\n", msg_in);
    char *p = strstr(msg_in, "Void Cascade");
    if (p && !is_cascade){
      is_cascade = 1;
      // printf("VOID CASCADE\n");

      notify_init("CASCADE");
      NotifyNotification* n = notify_notification_new ("Hop on Warframe", 
                                                       "Omnia void cascade just dropped",
                                                       0);
      notify_notification_set_timeout(n, 5000); // 5 seconds
      notify_notification_show(n, 0);

    }
    else if (!p && is_cascade){
      is_cascade = 0;
      // printf("VOID CASCADE\n");

      notify_init("NO CASCADE");
      NotifyNotification* m = notify_notification_new ("No more cascade for you",
                                                       "Too late now :(",
                                                       0);
      notify_notification_set_timeout(m, 5000); // 5 seconds
      notify_notification_show(m, 0);
    }

    free(msg_in);

    sleep(2);
  }

  return 0;
}
