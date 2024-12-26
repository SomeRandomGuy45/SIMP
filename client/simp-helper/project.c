#include  "project.h"
#include "options.h"

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void init_project_data(struct project_data *data) {
    create_folder(data->project_name);
    char* configFile = malloc(512);
    snprintf(configFile, 512, "%s/%s", data->project_name, "/config.json");
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "project_name", cJSON_CreateString(data->project_name));
    cJSON_AddItemToObject(json, "dependencies", cJSON_CreateArray());
    cJSON_AddItemToObject(json, "author", cJSON_CreateString(data->author));
    cJSON_AddItemToObject(json, "description", cJSON_CreateString(data->description));
    char* output = cJSON_Print(json);
    FILE* file = fopen(configFile, "w");
    fprintf(file, "%s", output);
    fclose(file);
    cJSON_Delete(json);
    free(configFile);
}