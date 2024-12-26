#include "cJSON.h"

char* URLS[6] = {
    "http://localhost:8080/",
    "",
    "",
    "",
    "",
    "",
};

int check_url_status();
char* curl_function(char *url);
void curl_function_download(char *url, char *path);
cJSON *get_project_data(int url, char* project_name);