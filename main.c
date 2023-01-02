#define _GNU_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/statvfs.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

char best_catalog_name[255];
unsigned long best_catalog_size = 0;

void process(char *target_path, unsigned long level, int *catalog_size) {
    int syscall_result;
    struct stat file_stats;

    char shift[255];
    shift[level * 2 + 1] = '\0';
    for (int i = 0; i <= level * 2; i++) {
        shift[i] = ' ';
    }

    DIR *directory_stream;
    directory_stream = opendir(target_path);

    if (directory_stream == NULL && errno != ENOTDIR) {
        printf("%s\n", strerror(errno));
        return;
    }

    if (directory_stream == NULL) {
        int path_descriptor = open(target_path, O_PATH);

        if (fstat(path_descriptor, &file_stats) == -1) {
            printf("%s\n", strerror(path_descriptor));
        } else {
            *catalog_size += file_stats.st_size;
        }
    } else {
        struct dirent *entry;

        entry = readdir(directory_stream);

        while (entry) {
            if (!strcmp(entry->d_name, "..") || !strcmp(entry->d_name, ".")) {
                entry = readdir(directory_stream);
                continue;
            }

            int subcatalog_size = 0;

            char child_path[255];
            strcpy(child_path, target_path);
            child_path[strlen(target_path)] = '/';
            strcpy(child_path + strlen(target_path) + 1, entry->d_name);

            process(child_path, level + 1, &subcatalog_size);

            printf("%sD: %s; Size: %d bytes\n", shift, child_path, subcatalog_size);

            if (subcatalog_size > best_catalog_size) {
                best_catalog_size = subcatalog_size;
                strcpy(best_catalog_name, child_path);
            }

            entry = readdir(directory_stream);
        }
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Не указан путь к целевому каталогу\n");
        exit(-1);
    }

    if (argc == 2) {
        printf("Не указан путь к файлу вывода\n");
        exit(-1);
    }

    int catalog_size;

    process(argv[1], 0, &catalog_size);
    if (catalog_size > best_catalog_size) {
        best_catalog_size = catalog_size;
        strcpy(best_catalog_name, argv[1]);
    }
    FILE *fout = fopen(argv[2], "w");

    char *full_path = realpath(best_catalog_name, NULL);
    fprintf(fout, "Biggest catalog: %s; Size: %d bytes\n", full_path, best_catalog_size);
    fclose(fout);

    return 0;
}
