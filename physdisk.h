#ifndef PHYSDISK
#define PHYSDISK

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define block_size  1024
#define max_file_name 1024

typedef struct
{
    unsigned long size; //in superblock
    //bool *block_state;  //in superblock

    unsigned long blocks_count;
    char *name;
    char *current_path;
    char current_block[block_size]; //block buffer
    FILE* desc;
}Disk;

bool create_disk(Disk *dsk, const char *disk_name, unsigned long disk_size);
bool open_disk(Disk *dsk, const char *filename);
void close_disk(Disk *dsk);
bool read_block(Disk *dsk, long disk_address, char *buff);
bool allocate_block(Disk *dsk, long disk_address, char *data_block);
bool update_super_block(Disk *dsk, long block_number, bool value);
bool delete_block(Disk *dsk, long block_number);

#endif //PHYSDISK