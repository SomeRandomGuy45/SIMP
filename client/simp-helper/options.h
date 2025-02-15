#ifdef _WIN32
    #include <direct.h>
    #include <limits.h>
    #define getcwd _getcwd
    #define PATH_MAX MAX_PATH
    #define create_folder(path) _mkdir(path)
    #define SIMP_TEMP_PATH = "C:\\Program Files\\simpsettings"
    #define UNZIP_COMMAND "powershell -Command \"Expand-Archive -Path '%s' -DestinationPath '%s'\""
    #define folder_exists(path) ((GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES) && (GetFileAttributesA(path) & FILE_ATTRIBUTE_DIRECTORY))
#else
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <limits.h>
    #define create_folder(path) mkdir(path, 0755)
    #define UNZIP_COMMAND "unzip -q \"%s\" -d \"%s\""
    #define folder_exists(path) ({ \
            struct stat info; \
            (stat((path), &info) == 0 && S_ISDIR(info.st_mode)) ? 1 : 0; \
    })
#endif

#ifdef __linux__
    #define SIMP_TEMP_PATH "~/.simpsettings"
#else
    #define SIMP_TEMP_PATH "/usr/local/lib/simpsettings"
#endif

extern int isDebug;

inline void set_debug_mode(int mode);