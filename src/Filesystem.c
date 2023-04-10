#include "Filesystem.h"

#include <libgen.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct FsScanDirResult FsScanDir(const char* path)
{
    struct dirent *entry;
    struct FsScanDirResult result = {0, NULL};

    if(path == NULL) {
        return result;
    }

    size_t pathLen = strlen(path);
    DIR *dir = opendir(path);

    if (dir != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            if(entry->d_type != DT_REG)continue;

            result.fileNames = (char **)realloc(result.fileNames, (result.count + 1) * sizeof(char *));

            // relative to absolute path
            char* fullPath = malloc(pathLen + strlen(entry->d_name) + 2);
            strcpy(fullPath, path);
            strcat(fullPath, "/");
            strcat(fullPath, entry->d_name);

            result.fileNames[result.count] = fullPath;
            ++result.count;
        } 
        closedir(dir);
    }

    return result;
}

void FsScanDir_Free(struct FsScanDirResult scanResult)
{
  for(int i=0; i<scanResult.count; ++i) {
    free(scanResult.fileNames[i]);
  }
  free(scanResult.fileNames);
}

bool FsIsDirectory(const char* path)
{
    struct stat pathStat;
    stat(path, &pathStat);
    return S_ISDIR(pathStat.st_mode);
}

bool FsFileExists(const char* path)
{
  return access(path, F_OK) == 0;
}