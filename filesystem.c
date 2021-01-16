#include "filesystem.h"

bool create_filesystem(Filesystem *fs, const char *disk_name, unsigned long disk_size)
{
    if(disk_size % block_size != 0)
    {
        perror("Bad align on create");
        return 0;
    }

    fs->blocks_count = disk_size / block_size;
    //allocate memory
    fs->name = malloc(max_file_name + 1);
    fs->current_path = malloc(max_file_name + 1);
    //****

    unsigned name_length = strlen(disk_name);
    if (name_length > max_file_name)
    {
        close_filesystem(fs);
        perror("Filename too long");
        return 0;
    }
    memcpy(fs->name, disk_name, name_length);
    fs->name[name_length] = 0;

    fs->size = disk_size;

    if( (fs->desc = fopen(disk_name, "r") )) //file exists already
    {
        perror("File already exists.");
        close_filesystem(fs);
        return 0;
    }

    // creating new file
    fs->desc = fopen(disk_name, "a+b");
    if(fs->desc == NULL)
        return 0;
    //****************


    // allocate superblock
    fwrite(&disk_size, sizeof(disk_size), 1, fs->desc); //disk size in superblock

    unsigned long i;
    for(i = 0; i < fs->blocks_count; i++)
    {
        char zero_byte = 0;
        fwrite(&zero_byte, 1, 1, fs->desc); //writing block state - whole disk is empty
    }
   //********************

    for(i = 0; i < disk_size ; i++)
    {
        char zero_byte = 0;
        fwrite(&zero_byte, 1, 1, fs->desc); //writing empty data to disk
    }

    fs->current_path[0] = '/';
    fs->current_path[1] = 0;

    fclose(fs->desc);
    return 1;
}

bool open_filesystem(Filesystem *fs, const char *filename)
{
    fs->desc = fopen(filename, "rb");
    if(fs->desc == NULL)
    {
        return 0;
    }

    //getting size of disk
    fread(&fs->size, sizeof(fs->size), 1, fs->desc );
    fs->blocks_count = fs->size / block_size;

    //allocate memory
    fs->name = malloc(max_file_name + 1);
    fs->current_path = malloc(max_file_name + 1);
    //fs->block_state = malloc(fs->blocks_count);
    //****

    memcpy(fs->name, filename, strlen(filename) + 1);

    //getting blocks state from superblock
    unsigned long i;
    for(i = 0; i < fs->blocks_count; i++)
    {
        char buff;
        fread(&buff, 1, 1, fs->desc);
        //fs->block_state[i] = (bool)buff;
    }

    fs->current_path[0] = '/';
    fs->current_path[1] = 0;

    fclose(fs->desc);
    return 1;
}

void close_filesystem(Filesystem *fs)
{
    free(fs->current_path);
    //free(fs->block_state);

    if(fs->desc)
        fclose(fs->desc);
}

bool read_block(Filesystem *fs, long disk_address, char *buff)
{
    if(disk_address % block_size != 0) //bad align
    {
        perror("Bad address align at reading");
        return 0;
    }

    fs->desc = fopen(fs->name, "rb");

    fseek(fs->desc, (long)(sizeof(fs->size) + fs->blocks_count + disk_address), SEEK_SET); //after superblock
    fread(buff, block_size, 1, fs->desc);

    fclose(fs->desc);
    return 1;
}

bool allocate_block(Filesystem *fs, long disk_address, char *data_block)
{
    if(disk_address % block_size != 0) // bad align
    {
        perror("bad address align at allocation");
        return 0;
    }

    fs->desc = fopen(fs->name, "r+b");
    if(fs->desc == NULL)
    {
        perror("Could not allocate block");
        return 0;
    }


    fseek(fs->desc, (long)(sizeof(fs->size) + fs->blocks_count + disk_address), SEEK_SET);
    fwrite(data_block, 1, block_size, fs->desc);

    unsigned int block_number = disk_address / block_size;
    //is the block free?
    bool is_allocated;
    fseek(fs->desc, (long)(sizeof(fs->size) + block_number), SEEK_SET);
    fread(&is_allocated, 1, 1, fs->desc);
    fclose(fs->desc);
    if(is_allocated)
        return 0;

    update_super_block(fs, (long)block_number, 1);
    return 1;
}

bool update_super_block(Filesystem *fs, long block_number, bool value)
{
    if(block_number > fs->blocks_count)
    {
        perror("wrong block number");
        return 0;
    }
    FILE *desc = fopen(fs->name, "r+b");
    if(desc == NULL)
        return 0;

    fseek(desc, (long)(sizeof(fs->size) + block_number), SEEK_SET);
    fwrite(&value, 1, 1, desc);

    fclose(fs->desc);
    return 1;
}

bool delete_block(Filesystem *fs, long disk_address)
{
    if(disk_address % block_size != 0)
    {
        return 0;
    }
    long block_number = disk_address / block_size;
    return update_super_block(fs, block_number, 0);
}