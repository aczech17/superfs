#include "filesystem.h"

#define GIGA 0x40000000
#define KILO 0x400

int main()
{
    const char *disk_name = "newdisk";
    Filesystem fs;
    bool disk_exists = open_filesystem(&fs, disk_name);
    if(disk_exists == 0)
    {
        bool create_success = create_filesystem(&fs, disk_name, GIGA);
        if(!create_success)
        {
            perror("Could not create disk");
            return 1;
        }
    }

    char new_block[block_size];
    memset(new_block, 255, block_size);
    long disk_address = 10 * block_size;

    bool alloc_success = allocate_block(&fs, disk_address, new_block);
    if(alloc_success)
        puts("Allocation successful");
    else
        puts("Allocation error");


    bool delete_success = delete_block(&fs, disk_address);
    if(delete_success)
        puts("Deleted");
    else
        puts("Error delete");

    alloc_success = allocate_block(&fs, disk_address, new_block);
    if(alloc_success)
        puts("Allocation successful");
    else
        puts("Allocation error");

    close_filesystem(&fs);
    return 0;
}
