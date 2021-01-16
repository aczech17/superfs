#ifndef FILESYSTEM
#define FILESYSTEM

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define block_size  1024
#define max_file_name 1024

struct Filesystem;

typedef struct
{
    unsigned int disk_address;
    unsigned int blocks;
    bool is_folder;
}I_node;

typedef struct
{
    unsigned long size; //in superblock
    bool *block_state;  //in superblock

    unsigned long blocks_count;
    char *name;
    char *current_path;
    char current_block[block_size]; //block buffer
    FILE* desc;
}Filesystem;

bool update_super_block(Filesystem *fs, unsigned int block_number, bool value);
bool create_filesystem(Filesystem *fs, const char *disk_name, unsigned long disk_size);
void close_filesystem(Filesystem *fs);
bool read_block(Filesystem *fs, long disk_address, char *buff);
bool allocate_block(Filesystem *fs, long disk_address, char *data_block);
bool update_super_block(Filesystem *fs, unsigned int block_number, bool value);
bool open_filesystem(Filesystem *fs, const char *filename);


#endif //FILESYSTEM