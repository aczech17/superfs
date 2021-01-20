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
    dsk->taken_bytes = 0;

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
    fwrite(&dsk->taken_bytes, sizeof(dsk->taken_bytes), 1, dsk->desc);

    unsigned long i;
    for(i = 0; i < dsk->blocks_count; i++)
    {
        byte zero_byte = 0;
        fwrite(&zero_byte, 1, 1, dsk->desc); //writing block state - whole disk is empty
    }
   //********************

    for(i = 0; i < disk_size ; i++)
    {
        byte zero_byte = 0;
        fwrite(&zero_byte, 1, 1, dsk->desc); //writing empty data to disk
    }

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

    //getting taken space
    fseek(dsk->desc, sizeof(dsk->size), SEEK_SET);
    fread(&dsk->taken_bytes, sizeof(dsk->taken_bytes), 1, dsk->desc);

    //allocate memory
    dsk->name = malloc(max_file_name + 1);
    //dsk->block_state = malloc(dsk->blocks_count);
    //****

    memcpy(dsk->name, filename, strlen(filename) + 1);

    fclose(dsk->desc);
    return 1;
}

void close_disk(Disk *dsk)
{
    free(dsk->name);
    if(dsk->desc)
        fclose(dsk->desc);
}

bool read_block(Disk *dsk, long disk_address, byte *buff)
{
    if(disk_address % block_size != 0) //bad align
    {
        perror("Bad address align at reading");
        return 0;
    }

    dsk->desc = fopen(dsk->name, "rb");

    fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes)
                            + dsk->blocks_count + disk_address), SEEK_SET); //after superblock and taken bytes
    fread(buff, block_size, 1, dsk->desc);

    fclose(dsk->desc);
    return 1;
}

bool allocate_block(Disk *dsk, long disk_address, byte *data_block)
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

    //is the block free?
    unsigned int block_number = disk_address / block_size;
    bool is_allocated;
    int error = fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes) + block_number), SEEK_SET);
    if(error)
    {
        perror("error");
        return 0;
    }
    fread(&is_allocated, 1, 1, dsk->desc);
    if(is_allocated)
    {
        fclose(dsk->desc);
        return 0;
    }


    error = fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes) +
                                            dsk->blocks_count + disk_address), SEEK_SET);
    if(error)
    {
        perror("error");
        return 0;
    }
    fwrite(data_block, 1, block_size, dsk->desc);
    fclose(dsk->desc);


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

    //updating taken bytes
    if(value == 1)
    {
        if (dsk->taken_bytes < dsk->size)
            dsk->taken_bytes += block_size;
        else
        {
            fclose(dsk->desc);
            return 0;
        }
    }
    if (value == 0)
    {
        if (dsk->taken_bytes > 0)
            dsk->taken_bytes -= block_size;
        else
        {
            fclose(dsk->desc);
            return 0;
        }
    }
    fseek(desc, (long)(sizeof(dsk->size)), SEEK_SET);
    fwrite(&dsk->taken_bytes, 1, sizeof(dsk->taken_bytes), dsk->desc);
    //**********

    fseek(desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes) + block_number), SEEK_SET);
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
    bool update_success = update_super_block(dsk, block_number, 0);
    if(!update_success)
        return 0;
    return 1;
}

bool is_free(Disk *dsk, long block_number)
{
    dsk->desc = fopen(dsk->name, "rb");
    fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes) + block_number), SEEK_SET);
    bool result;
    fread(&result, 1, 1, dsk->desc);

    fclose(dsk->desc);
    return result;
}

long find_blocks(Disk *dsk, long blocks_wanted)
{
    if(blocks_wanted > dsk->blocks_count)
        return -1;

    dsk->desc = fopen(dsk->name, "rb");
    fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes)), SEEK_SET);
    byte *block_info = malloc(dsk->blocks_count);
    fread(block_info, 1, dsk->blocks_count, dsk->desc);

    long potential_address = 0;
    long i;
    for(i = 0; i < dsk->blocks_count - blocks_wanted; i++)
    {
        if(block_info[i] == 0)
        {
            potential_address = i;
            long j;
            for(j = potential_address; j < potential_address + blocks_wanted; j++)
            {
                if(j == dsk->blocks_count || block_info[j] == 1) // j is allocated or end of array
                {
                    i = j + 1;
                    break;
                }
            }
            if(j == potential_address + blocks_wanted) //we did it
            {
                fclose(dsk->desc);
                free(block_info);
                return potential_address;
            }
        }//if
    }
    fclose(dsk->desc);
    free(block_info);
    return -1; //could not find
}

void map_disk(Disk *dsk)
{
    byte *block_info = malloc(dsk->blocks_count);

    dsk->desc = fopen(dsk->name, "rb");
    fseek(dsk->desc, (long)(sizeof(dsk->size) + sizeof(dsk->taken_bytes)), SEEK_SET);
    fread(block_info, 1, dsk->blocks_count, dsk->desc);

    long current_block = 0;
    bool current_state = block_info[0];
    long i;
    for(i = 0; i < dsk->blocks_count; i++)
    {
        if(block_info[i] != current_state || i == dsk->blocks_count - 1)
        {
            long address = current_block * block_size;
            long space_size = (i - current_block) * block_size;
            printf("Address: %lu ", address);
            printf("Size: %lu ", space_size);
            printf("State: %s\n", current_state == 1 ? "taken" : "free");

            current_block = i;
            current_state = block_info[i];
        }
    }
    free(block_info);
    fclose(dsk->desc);
}