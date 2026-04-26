#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

bool checkFolderName(const char *rootPath, const char *fileName);
bool checkIfHasJar(const char *path);

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

        if (entry == NULL) {
            break;
        }
        if (serverCount >= 127) {
            printf("Over 128 servers found, aborting.\n");
            return 1;
        }
        if (checkFolderName(rootServerPath, entry->d_name)) {
            char fullPath[512];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", rootServerPath, entry->d_name);
            if (checkIfHasJar(fullPath)) {
                snprintf(serverPaths[serverCount], 512, "%s/%s",rootServerPath, entry->d_name);
                serverCount++;
            }
        }
    }

    closedir(rootDir);

    for (int i = 0; i < serverCount; i++) {
        printf("%s\n", serverPaths[i]);
    }

    printf("%d\n", serverCount);

    return 0;
}

bool checkFolderName(const char *rootPath, const char *fileName) {
    if (strcmp(fileName, ".") == 0) return false;
    if (strcmp(fileName, "..") == 0) return false;

    char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", rootPath, fileName);

    struct stat st;
    if (stat(fullPath, &st) != 0) return false;
    if (!S_ISDIR(st.st_mode)) return false;

    return true;
}

bool checkIfHasJar(const char *path) {
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
