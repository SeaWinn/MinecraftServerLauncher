#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>

#define RESET   "\x1b[0m"
#define RED     "\x1b[31m"
#define BOLD    "\x1b[1m"

bool isAValidFolder(const char *rootPath, const char *fileName);
bool folderHasJar(const char *path);

int main(void) {
    char rootServerPath[512];
    char serverPaths[128][512];
    int serverCount = 0;

    memset(serverPaths, 0, sizeof(serverPaths));

    do {
        printf("Enter your Minecraft servers' path: \n");
        fgets(rootServerPath, sizeof(rootServerPath), stdin);
        rootServerPath[strcspn(rootServerPath, "\n")] = '\0';
    }while(rootServerPath[0] == '\0');

    DIR * rootDir = opendir(rootServerPath);
    printf("Opening %s...\n", rootServerPath);
    if (rootDir == NULL) {
        perror("Error opening directory");
        return 1;
    }

    while (true) {
        const struct dirent *entry = readdir(rootDir);

        if (entry == NULL) break;
        if (serverCount >= 127) {
            printf("Over 128 servers found, aborting.\n");
            return 1;
        }
        if (isAValidFolder(rootServerPath, entry->d_name)) {
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", rootServerPath, entry->d_name);
            if (folderHasJar(fullPath)) {
                snprintf(serverPaths[serverCount], 512, "%s/%s",rootServerPath, entry->d_name);
                serverCount++;
            }
        }
    }

    closedir(rootDir);

    int pickedServer;
    for (int i = 0; i < serverCount; i++) {
        printf(BOLD RED "[%d]" RESET BOLD " : %s\n" RESET,i, strrchr(serverPaths[i], '/'));
    }

    char buffer[64];
    while (true) {
        fgets(buffer, sizeof(buffer), stdin);

        char *endptr;
        const long value = strtol(buffer, &endptr, 10);

        if (endptr == buffer) {
            printf("Error: Not a number.\n");
            continue;
        }
        if (*endptr != '\n' && *endptr != '\0') {
            printf("Error: Invalid characters in input.\n");
            continue;
        }
        if (value < 0 || value >= serverCount) {
            printf("Error: Number must be between 0 and %d.\n", serverCount - 1);
            continue;
        }
        pickedServer = (int)value;
        break;
    }
    printf("Launching %s...\n",strrchr(serverPaths[pickedServer], '/'));
    return 0;
}

bool isAValidFolder(const char *rootPath, const char *fileName) {
    if (strcmp(fileName, ".") == 0) return false;
    if (strcmp(fileName, "..") == 0) return false;

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", rootPath, fileName);

    struct stat st;
    if (stat(fullPath, &st) != 0) return false;
    if (!S_ISDIR(st.st_mode)) return false;

    return true;
}

bool folderHasJar(const char *path) {
    DIR * iteratedDir = opendir(path);
    if (iteratedDir == NULL) return false;

    while(true) {
        const struct dirent *entry = readdir(iteratedDir);
        if (entry == NULL) break;

        const char *ext = strrchr(entry->d_name, '.');
        if (ext != NULL && strcmp(ext, ".jar") == 0) {
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

            struct stat st;
            if (stat(fullPath, &st) == 0 && S_ISREG(st.st_mode)) {
                closedir(iteratedDir);
                return true;
            }
        }
    }
    closedir(iteratedDir);
    return false;

}
