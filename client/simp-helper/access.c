#include "access.h"

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    char **response = (char **)userdata;

    // Allocate memory to store the data
    *response = realloc(*response, total_size + 1);
    if (*response == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 0;  // Return 0 to stop the transfer
    }

    // Copy the data into the buffer
    memcpy(*response, ptr, total_size);
    (*response)[total_size] = '\0';  // Null-terminate the string

    return total_size;
}

char* curl_function(char *url) {
    CURL *curl;
    CURLcode res;
    char *response_data = NULL;

    // Initialize libcurl
    curl = curl_easy_init();
    if(curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Write the response to stdout
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if(res != CURLE_OK) { 
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }
    return response_data;
}

int check_url_status() {
    for (int i = 0; i < 6; i++) {
        char *url = URLS[i];
        if (strlen(url) == 0) continue;
        char *response = curl_function(url);
        if (response) {
            free(response);
            return i;
        }
    }
    return -1;
}

cJSON *get_project_data(int url_index, char *project_name) {
    // Ensure URLS array and project_name are valid
    if (url_index < 0 || project_name == NULL) return NULL;

    // Retrieve the base URL from the URLS array
    const char *base_url = URLS[url_index];
    if (strlen(base_url) == 0) return NULL;

    // Construct the full URL
    char full_url[512];  // Adjust the size as needed
    snprintf(full_url, sizeof(full_url), "%sapi/project-data/%s", base_url, project_name);

    // Fetch the response using curl_function
    char *response = curl_function(full_url);
    if (response == NULL) {
        return NULL;  // Handle the case where the request fails
    }

    // Parse the JSON response
    cJSON *json_data = cJSON_Parse(response);

    // Free the response buffer
    free(response);

    // Return the parsed JSON object
    return json_data;
}