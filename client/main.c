#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simp-helper/simp.h"

void show_usage() {
    printf("simp [option]\n Options:\n    -h, --help     Show this help message\n    -c, create   Create a new project\n    -i, --install  Install project dependencies\n   -op, --option Set SIMP option\n");
}

int run_options(char **argv) {
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        show_usage();
        return 0;
    } else if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "create") == 0) {
        return 1;
    } else if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--install") == 0) {
        return 2;
    } else if (strcmp(argv[1], "-op") == 0 || strcmp(argv[1], "--option") == 0) {
        return 3;
    }
    return 0;
};

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
    if (shouldQuit == 2) {
        cJSON *jsonData = get_project_data(urlNum, argv[2]);
        if (jsonData == NULL) {
            fprintf(stderr, "Failed to retrieve project data. Exiting...\n");
            cJSON_free(jsonData);
            return -1;
        }
        
        cJSON_free(jsonData);
    }
    return 0;
}