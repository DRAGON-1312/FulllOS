#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(2, "Usage: %s mask command\n", argv[0]);
        exit(1);
    }

    // atoi(argv[1]): convert string to int
    if(trace(atoi(argv[1])) < 0){
        fprintf(2, "%s: trace failed\n", argv[0]);
        exit(1);
    }

    // argv + 2: trỏ tới mảng bắt đầu từ "grep"
    exec(argv[2], argv + 2);
    exit(0);
}
