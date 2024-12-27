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
int curl_upload(const char* access_token, const char* server_url, const char* project_name, const char* upload_file);
int zip_files(const char* source_dir, const char* zip_file, int useCD);
char* curl_function(char *url);
char* curl_function_download(char *url, char *path);
char* curl_function_reglog(const char *url, const char *username, const char *password);
void extract_file(const char *filename, const char *output_dir);
cJSON *get_project_data(int url, char* project_name); 