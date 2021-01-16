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


    close_filesystem(&fs);
    return 0;
}
