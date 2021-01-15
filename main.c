#include "filesystem.h"

int main()
{
    Filesystem fs;
    bool result = create_filesystem(&fs, "newdisk");
    printf("%d\n", result);


    close_filesystem(&fs);
    return 0;
}
