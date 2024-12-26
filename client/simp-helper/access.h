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
cJSON *get_project_data(int url, char* project_name);