#ifndef PHYSDISK
#define PHYSDISK

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define block_size      1024
#define max_file_name   1024

typedef uint8_t byte;

typedef struct
{
    unsigned long size; //in superblock
    unsigned long taken_bytes;

    unsigned long blocks_count;
    char *name;
    byte current_block[block_size]; //block buffer
    FILE* desc;
}Disk;

bool create_disk(Disk *dsk, const char *disk_name, unsigned long disk_size);
bool open_disk(Disk *dsk, const char *filename);
void close_disk(Disk *dsk);
bool read_block(Disk *dsk, long disk_address, byte *buff);
bool allocate_block(Disk *dsk, long disk_address, byte *data_block);
bool update_super_block(Disk *dsk, long block_number, bool value);
bool delete_block(Disk *dsk, long block_number);
bool is_free(Disk *dsk, long block_number);
long find_blocks(Disk *dsk, long blocks_wanted);
void map_disk(Disk *dsk);

#endif //PHYSDISK