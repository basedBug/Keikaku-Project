#pragma once

//#include <Arduino.h>
#include <LittleFS.h>
#include <FS.h>

void initFs();

/**
 * @brief Recursively lists the contents of a directory in the specificied filesystem
 * 
 * @param fs Reference to the filesystem object
 * @param dirname Path of the directory to list (null-terminated string)
 * @param levels Depth of recursion for subdirectories (0 = current dir only)
 * 
 * @note The function will print directory contents to serial output
 * @note For directories, it will recursively list contents if levels > 0
 * @note For files, it displays name and size in both KB and bytes
 * @warning Deep recursion may impact stack stability
 */
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void writeFile2(fs::FS &fs, const char *path, const char *message);
void deleteFile2(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);

/**
 * @brief Converts bytes to kilobytes (KB) using binary (1024-byte) units
 * @param bytes Size in bytes (e.g., from file.size())
 * @return float_t Kilobytes as a float (1 KB = 1024 bytes)
 * 
 * @note Returns fractional values (e.g., 1536 bytes → 1.500 KB)
 */
float_t bytesToKB(const size_t bytes);
