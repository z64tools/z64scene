#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>

struct FsScanDirResult {
  int count;
  char **fileNames;
};

/**
 * @brief Scans a directory and list all files
 * NOTE: call FsScanDir_Free() to free the results memory
 * 
 * @param path directory to scan
 * @return struct FsScanDirResult result with absolute paths
 */
struct FsScanDirResult FsScanDir(const char* path);

/**
 * @brief Frees memory from FsScanDir()
 * 
 * @param scanResult result to free
 */
void FsScanDir_Free(struct FsScanDirResult scanResult);

/**
 * @brief Checks if a given path is a directory
 * 
 * @param path 
 * @return true if directory
 */
bool FsIsDirectory(const char* path);

#endif