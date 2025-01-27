#ifndef HLS_H
#define HLS_H

#include <dirent.h>
#include <sys/stat.h>

void print_directory_contents(const char *directory);
int open_directory(const char *directory, DIR **dir);
void read_directory_entries(DIR *dir);
void print_err(const char *program, const char *path);
void if_path(const char *path, const char *program);

#endif /* HLS_H */
