#include "physdisk.h"

bool create_disk(Disk *dsk, const char *disk_name, unsigned long disk_size)
{
    if(disk_size % block_size != 0)
    {
        perror("Bad align on create");
        return 0;
    }

    dsk->blocks_count = disk_size / block_size;
    //allocate memory
    dsk->name = malloc(max_file_name + 1);
    dsk->current_path = malloc(max_file_name + 1);
    //****

    unsigned name_length = strlen(disk_name);
    if (name_length > max_file_name)
    {
        close_disk(dsk);
        perror("Filename too long");
        return 0;
    }
    memcpy(dsk->name, disk_name, name_length);
    dsk->name[name_length] = 0;

    dsk->size = disk_size;

    if( (dsk->desc = fopen(disk_name, "r") )) //file exists already
    {
        perror("File already exists.");
        close_disk(dsk);
        return 0;
    }

    // creating new file
    dsk->desc = fopen(disk_name, "a+b");
    if(dsk->desc == NULL)
        return 0;
    //****************


    // allocate superblock
    fwrite(&disk_size, sizeof(disk_size), 1, dsk->desc); //disk size in superblock

    unsigned long i;
    for(i = 0; i < dsk->blocks_count; i++)
    {
        char zero_byte = 0;
        fwrite(&zero_byte, 1, 1, dsk->desc); //writing block state - whole disk is empty
    }
   //********************

    for(i = 0; i < disk_size ; i++)
    {
        char zero_byte = 0;
        fwrite(&zero_byte, 1, 1, dsk->desc); //writing empty data to disk
    }

    dsk->current_path[0] = '/';
    dsk->current_path[1] = 0;

    fclose(dsk->desc);
    return 1;
}

bool open_disk(Disk *dsk, const char *filename)
{
    dsk->desc = fopen(filename, "rb");
    if(dsk->desc == NULL)
        return 0;

    //getting size of disk
    fread(&dsk->size, sizeof(dsk->size), 1, dsk->desc );
    dsk->blocks_count = dsk->size / block_size;

    //allocate memory
    dsk->name = malloc(max_file_name + 1);
    dsk->current_path = malloc(max_file_name + 1);
    //dsk->block_state = malloc(dsk->blocks_count);
    //****

    memcpy(dsk->name, filename, strlen(filename) + 1);

    //getting blocks state from superblock
    unsigned long i;
    for(i = 0; i < dsk->blocks_count; i++)
    {
        char buff;
        fread(&buff, 1, 1, dsk->desc);
    }

    dsk->current_path[0] = '/';
    dsk->current_path[1] = 0;

    fclose(dsk->desc);
    return 1;
}

void close_disk(Disk *dsk)
{
    free(dsk->current_path);
    if(dsk->desc)
        fclose(dsk->desc);
}

bool read_block(Disk *dsk, long disk_address, char *buff)
{
    if(disk_address % block_size != 0) //bad align
    {
        perror("Bad address align at reading");
        return 0;
    }

    dsk->desc = fopen(dsk->name, "rb");

    fseek(dsk->desc, (long)(sizeof(dsk->size) + dsk->blocks_count + disk_address), SEEK_SET); //after superblock
    fread(buff, block_size, 1, dsk->desc);

    fclose(dsk->desc);
    return 1;
}

bool allocate_block(Disk *dsk, long disk_address, char *data_block)
{
    if(disk_address % block_size != 0) // bad align
    {
        perror("bad address align at allocation");
        return 0;
    }

    dsk->desc = fopen(dsk->name, "r+b");
    if(dsk->desc == NULL)
    {
        perror("Could not allocate block");
        return 0;
    }


    fseek(dsk->desc, (long)(sizeof(dsk->size) + dsk->blocks_count + disk_address), SEEK_SET);
    fwrite(data_block, 1, block_size, dsk->desc);

    unsigned int block_number = disk_address / block_size;
    //is the block free?
    bool is_allocated;
    fseek(dsk->desc, (long)(sizeof(dsk->size) + block_number), SEEK_SET);
    fread(&is_allocated, 1, 1, dsk->desc);
    fclose(dsk->desc);
    if(is_allocated)
        return 0;

    update_super_block(dsk, (long)block_number, 1);
    return 1;
}

bool update_super_block(Disk *dsk, long block_number, bool value)
{
    if(block_number > dsk->blocks_count)
    {
        perror("wrong block number");
        return 0;
    }
    FILE *desc = fopen(dsk->name, "r+b");
    if(desc == NULL)
        return 0;

    fseek(desc, (long)(sizeof(dsk->size) + block_number), SEEK_SET);
    fwrite(&value, 1, 1, desc);

    fclose(dsk->desc);
    return 1;
}

bool delete_block(Disk *dsk, long disk_address)
{
    if(disk_address % block_size != 0)
    {
        return 0;
    }
    long block_number = disk_address / block_size;
    return update_super_block(dsk, block_number, 0);
}