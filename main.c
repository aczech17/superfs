#include "physdisk.h"

#define GIGA 0x40000000
#define KILO 0x400

int main()
{
    bool success;
    const char *disk_name = "newdisk";
    Disk dsk;
    bool disk_exists = open_disk(&dsk, disk_name);
    if(disk_exists == 0)
    {
        bool create_success = create_disk(&dsk, disk_name, GIGA);
        if(!create_success)
        {
            perror("Could not create disk");
            return 1;
        }
    }

    char new_block[block_size];
    memset(new_block, 255, block_size);
    long disk_address = 0;

    success = delete_block(&dsk, disk_address);
    if(success)
        puts("Deleted");
    else
        puts("Error delete");


    success = allocate_block(&dsk, disk_address, new_block);
    if(success)
        puts("Allocation successful");
    else
        puts("Allocation error");

    success = allocate_block(&dsk, disk_address, new_block);
    if(success)
        puts("Allocation successful");
    else
        puts("Allocation error");


    printf("%lu\n", sizeof(dsk.size) + sizeof(dsk.taken_bytes) + dsk.blocks_count);
    close_disk(&dsk);
    return 0;
}
