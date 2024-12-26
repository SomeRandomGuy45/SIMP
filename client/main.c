#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "simp-helper/simp.h"

void show_usage() {
    printf("simp [option]\n Options:\n    h, help     Show this help message\n    c, create   Create a new project\n    i, install  Install project dependencies\n  op, option Set SIMP option\n");
}

int run_options(char **argv) {
    if (strcmp(argv[1], "h") == 0 || strcmp(argv[1], "help") == 0) {
        show_usage();
        return 0;
    } else if (strcmp(argv[1], "c") == 0 || strcmp(argv[1], "create") == 0) {
        return 1;
    } else if (strcmp(argv[1], "i") == 0 || strcmp(argv[1], "install") == 0) {
        return 2;
    } else if (strcmp(argv[1], "op") == 0 || strcmp(argv[1], "option") == 0) {
        return 3;
    }
    return 0;
};

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if (result == NULL) {
        printf("Failed to allocate memory for concatenation. Exiting...\n");
        exit(EXIT_FAILURE);
    }
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

int main(int argc, char *argv[]) {
    if (!folder_exists(SIMP_TEMP_PATH)) {
        if (create_folder(SIMP_TEMP_PATH) != 0) {
            perror("Failed to create setting directory. Exiting...\n");
            return -1;
        }
    }
    if (argc < 2) {
        show_usage();
        return 0;
    }
    int shouldQuit = run_options(argv);
    if (shouldQuit == 0) {
        return 0;
    }
    int urlNum = check_url_status();
    if (urlNum == -1) {
        perror("Failed to access all URLs. Exiting...\n");
        return -1;
    }
    char *username = malloc(256 * sizeof(char));
    char *password = malloc(256 * sizeof(char));
    printf("Enter username: ");
    scanf("%255s", username); // Limit input to prevent overflow
    printf("Enter password: ");
    scanf("%255s", password); // Limit input to prevent overflow
    char *regURL = concat(URLS[urlNum], "register");
    curl_function_reglog(regURL, username, password);
    free(regURL);
    char *loginURL = concat(URLS[urlNum], "login");
    char *loginTokenData = curl_function_reglog(loginURL, username, password);
    cJSON* loginTokenJSON = cJSON_Parse(loginTokenData);
    if (isDebug) {
        printf("loginToken received: %s\n", cJSON_GetObjectItem(loginTokenJSON, "token")->valuestring);
    }
    free(loginTokenData);
    free(loginURL);
    free(username);
    free(password);
    if (shouldQuit == 1) {
        struct project_data *data = malloc(sizeof(struct project_data));
        if (data == NULL) {
            perror("Failed to allocate memory for project data");
            return -1;
        }

        // Allocate memory for project_name and author
        data->project_name = malloc(256 * sizeof(char));
        data->author = malloc(256 * sizeof(char));
        data->description = malloc(1024 * sizeof(char));

        if (data->project_name == NULL || data->author == NULL) {
            perror("Failed to allocate memory for project name or author");
            free(data); // Free the struct before exiting
            return -1;
        }

        printf("Enter project name: ");
        scanf("%255s", data->project_name); // Limit input to prevent overflow
        printf("Enter author name: ");
        scanf("%255s", data->author); // Limit input to prevent overflow
        printf("Enter project description: ");
        scanf("%1024s", data->description);

        init_project_data(data);

        zip_files(data->project_name, data->project_name);
        curl_upload(cJSON_GetObjectItem(loginTokenJSON, "token")->valuestring, URLS[urlNum], data->project_name, data->project_name);
        free(data);
    } else if (shouldQuit == 2) {
        if (!folder_exists("simple_modules")) {
            if (create_folder("simple_modules")) {
                perror("Failed to create 'simple_modules' directory. Exiting...\n");
                return -1;
            }
        }
        cJSON *jsonData = get_project_data(urlNum, argv[2]);
        if (jsonData == NULL) {
            fprintf(stderr, "Failed to retrieve project data. Exiting...\n");
            return -1;
        }

        cJSON *urlItem = cJSON_GetObjectItem(jsonData, "url");
        char* url = (urlItem != NULL) ? urlItem->valuestring : NULL;

        cJSON *pathItem = cJSON_GetObjectItem(jsonData, "projectName");
        char* path = (pathItem != NULL) ? pathItem->valuestring : NULL;

        if (url == NULL || path == NULL) {
            fprintf(stderr, "Missing required data in the JSON. Exiting...\n");
            cJSON_free(jsonData);
            return -1;
        }

        cJSON_free(jsonData);
        char *FullURL = concat(URLS[urlNum], url);
        char *InstallFilePath = concat("simple_modules/", argv[2]);
        char *FilePath = curl_function_download(FullURL, path);
        free(FullURL);
        extract_file(FilePath, InstallFilePath);
        free(FilePath);
        free(InstallFilePath);
    } else if (shouldQuit == 3) {
        // TODO
    }
    cJSON_Delete(loginTokenJSON);
    return 0;
}