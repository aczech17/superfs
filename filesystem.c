#include "filesystem.h"

bool create_filesystem(Filesystem *fs, const char *disk_name)
{
    //allocate memory
    fs->current_path = malloc(max_file_name + 1);
    fs->name = malloc(max_file_name + 1);
    fs->block_state = malloc(max_block);
    //****

    unsigned name_length = strlen(disk_name);
    if(name_length > max_file_name)
        return 0;

    memcpy(fs->name, disk_name, name_length);
    fs->name[name_length] = 0;
    fs->size = 0;

    memset(fs->block_state, 0, max_block); // all blocks are free

    if( (fs->desc = fopen(disk_name, "r") )) //file exists already
    {
        perror("File already exists.");
        fclose(fs->desc);
        return 0;
    }

    // creating new file
    fs->desc = fopen(disk_name, "ab");
    if(fs->desc == NULL)
        return 0;
    //****************

    fwrite(fs->block_state, 1, max_block, fs->desc); //writing super block to disk
    return 1;
}

void close_filesystem(Filesystem *fs)
{
    free(fs->current_path);
    free(fs->name);
    free(fs->block_state);
    fclose(fs->desc);
}

bool read_block(Filesystem *fs, long disk_address, char *buff)
{
    if(disk_address % block_size != 0) //bad align
    {
        perror("bad address align at reading");
        return 0;
    }
    fseek(fs->desc, disk_address + max_block, SEEK_SET); //after superblock
    fread(buff, block_size, 1, fs->desc);
    return 1;
}

bool allocate_block(Filesystem *fs, long disk_address, char *data_block)
{
    if(disk_address % block_size != 0) // bad align
    {
        perror("bad address align at allocation");
        return 0;
    }

    FILE *desc = fopen(fs->name, "wb");
    if(desc == NULL)
        return 0;

    fseek(desc, disk_address + max_block, SEEK_SET);
    fwrite(data_block, block_size, 1, desc);

    unsigned int block_number = disk_address / block_size;
    fs->block_state[block_number] = 1;
    update_super_block(fs, block_number, 1);

    fs->size += block_size;

    return 1;
}

bool update_super_block(Filesystem *fs, unsigned int block_number, bool value)
{
    if(block_number > max_block)
    {
        perror("wrong block number");
        return 0;
    }
    FILE *desc = fopen(fs->name, "wb");
    if(desc == NULL)
        return 0;

    fseek(desc, (long int)block_number, SEEK_SET);
    fwrite(&value, 1, 1, desc);

    return 1;
}