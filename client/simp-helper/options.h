#ifdef _WIN32
    #include <direct.h>
    #define create_folder(path) _mkdir(path)
    #define SIMP_TEMP_PATH = "C:\\Program Files\\simpsettings"
    int folder_exists(const char *path) {
        DWORD attributes = GetFileAttributes(path);
        if (attributes == INVALID_FILE_ATTRIBUTES) {
            return 0; // Path does not exist
        }
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #define create_folder(path) mkdir(path, 0755)
    #define SIMP_TEMP_PATH "/usr/local/lib/simpsettings"
    int folder_exists(const char *path) {
        struct stat info;

        // Check if the path exists and is a directory
        if (stat(path, &info) == 0 && S_ISDIR(info.st_mode)) {
            return 1; // Folder exists
        } else {
            return 0; // Folder does not exist
        }
    }
#endif

int isDebug = 0;

void set_debug_mode(int mode);