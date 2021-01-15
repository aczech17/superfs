#ifndef FILESYSTEM
#define FILESYSTEM

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define block_size  1024
#define max_file_name 1024
#define max_block      4194304 //4M clusters

struct Filesystem;

typedef struct
{
    unsigned int disk_address;
    unsigned int blocks;
    bool is_folder;
}I_node;

typedef struct
{
    char *current_path;
    char *name;
    int size;
    bool *block_state;
    char current_block[block_size];
    FILE* desc;
}Filesystem;

bool update_super_block(Filesystem *fs, unsigned int block_number, bool value);
bool create_filesystem(Filesystem *fs, const char *disk_name);
void close_filesystem(Filesystem *fs);
bool read_block(Filesystem *fs, long disk_address, char *buff);
bool allocate_block(Filesystem *fs, long disk_address, char *data_block);
bool update_super_block(Filesystem *fs, unsigned int block_number, bool value);


#endif //FILESYSTEM