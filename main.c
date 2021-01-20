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

    allocate_block(&dsk, 0, new_block);
    allocate_block(&dsk, 1024, new_block);
    allocate_block(&dsk, 5 * 1024, new_block);


    map_disk(&dsk);

    close_disk(&dsk);
    return 0;
}
