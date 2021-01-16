#include "filesystem.h"

#define GIGA 0x40000000
#define KILO 0x400

int main()
{
    Filesystem fs;
    //bool result = create_filesystem(&fs, "newdisk", GIGA);
    //printf("%d\n", result);

    bool result = open_filesystem(&fs, "newdisk");
    if(result == 0)
    {
        perror("Gowno");
        return 2137;
    }
    printf("%d\n", fs.size);
    printf("%d\n", fs.block_state[0]);

    char new_block[block_size];
    memset(new_block, 255, block_size);
    bool udalosiealokowackurwa = allocate_block(&fs, 0, new_block);
    printf("%d\n", udalosiealokowackurwa);


    close_filesystem(&fs);
    return 0;
}
