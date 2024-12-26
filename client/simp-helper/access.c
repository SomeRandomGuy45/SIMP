#include "access.h"
#include "options.h"

#ifdef _WIN32
    #define ZIP_COMMAND "powershell.exe Compress-Archive -Force -Path \"%s\" -DestinationPath \"%s\""
#elif __APPLE__
    #define ZIP_COMMAND "zip -r \"%s.zip\" \"%s\"/"
#elif __linux__
    #define ZIP_COMMAND "zip -r \"%s.zip\" \"%s\"/"
#endif

#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int zip_files(const char* source_dir, const char* zip_file) {
    char command[1024];

    // Construct the command string
    snprintf(command, sizeof(command), ZIP_COMMAND, source_dir, zip_file);

    // Execute the command using system()
    int result = system(command);

    if (result != 0) {
        fprintf(stderr, "Error: Failed to create zip archive.\n");
        return 1;
    }

    return 0;
}


int curl_upload(const char* access_token, const char* server_url, const char* project_name, const char* upload_file) {
    char command[2048];

    snprintf(command, sizeof(command),
           "curl -X POST "
           "-H \"Authorization: Bearer %s\" "
           "-F \"file=@%s.zip\" "
           "%sapi/project-data/%s/upload",
           access_token, upload_file, server_url, project_name);
    int result = system(command);

    if (result != 0) {
        fprintf(stderr, "Error: Failed to upload file using curl.\n");
        return 1;
    }

    return 0;
}

size_t write_callback_function(void *ptr, size_t size, size_t nmemb, void *userdata) {
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

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    if (written != size * nmemb) {
        fprintf(stderr, "Error writing data to file\n");
    }
    return written;
}

void extract_file(const char *filename, const char *output_dir) {
    char command[1024];
    
    // Prepare the unzip command
    snprintf(command, sizeof(command), UNZIP_COMMAND, filename, output_dir);

    // Execute the command
    int result = system(command);

    if (result != 0) {
        fprintf(stderr, "Failed to extract '%s' to '%s'. Command executed: %s\n", filename, output_dir, command);
    }
    remove(filename);
}

char* curl_function(char *url) {
    CURL *curl;
    CURLcode res;
    char *response_data = NULL;

    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_function);
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

char* curl_function_download(char *url, char *path) {
    CURL *curl;
    CURLcode res;
    FILE *file;
    
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        return "";
    }
    char *full_path = malloc(512);
    snprintf(full_path, 512, "%s/%s.zip", "simple_modules", path);
    file = fopen(full_path, "wb");
    if (!file) {
        perror("Error opening file");
        curl_easy_cleanup(curl);
        return "";
    }

    // Set URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

    res = curl_easy_perform(curl);
    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    // Clean up
    fclose(file);
    curl_easy_cleanup(curl);
    return full_path;
}

char* curl_function_reglog(const char *url, const char *username, const char *password) {
    CURL *curl;
    CURLcode res;
    char *response_data = malloc(1024); // Allocate memory for response
    if (response_data == NULL) {
        fprintf(stderr, "Failed to allocate memory for response data\n");
        return NULL;
    }
    memset(response_data, 0, 1024);  // Initialize response data to an empty string

    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "username", username);
    cJSON_AddStringToObject(json, "password", password);
    // Initialize libcurl
    curl = curl_easy_init();
    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // Set HTTP method to POST
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        // Set content type to JSON
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        // Set the JSON data as the POST body
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, cJSON_Print(json));
        // Set callback function to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_function);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data); 
        // Perform the request
        res = curl_easy_perform(curl);
        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            free(response_data);  // Free memory in case of error
            curl_easy_cleanup(curl);
            cJSON_Delete(json);
            return NULL;
        }
        // Clean up
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    cJSON_Delete(json);
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