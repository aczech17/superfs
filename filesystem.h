#ifndef SUPERFS_FILESYSTEM_H
#define SUPERFS_FILESYSTEM_H

#include "physdisk.h"

const char* concat(const char *str1, const char *str2)
{
    unsigned long long length_1 = strlen(str1);
    unsigned long long length_2 = strlen(str2);

    memcpy(str1 + length_1, str2, length_2);
    memset(str1 + length_1 + length_2, 0, 1);

    return str1;
}

typedef struct
{

}Inode;

typedef struct
{
    Disk* disk;
    char* current_path;
}Filesystem;

bool create_filesystem(Filesystem *fs, Disk *dsk)
{
    fs->disk = dsk;
}

#endif //SUPERFS_FILESYSTEM_H