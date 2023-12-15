#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("no file name\n");
        return 0;
    }

    char command[strlen(argv[1]) + 5];
    int fileDescriptor;
    if ((fileDescriptor = open(argv[1], O_RDWR)) == -1) {
        perror("can not open file");
        return 0;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(fileDescriptor, F_SETLK, &lock) == -1) {
        if (errno == EAGAIN) {
            printf("%s file read is locked\n", argv[1]);
            return 0;
        }
        perror("failed to lock file");
        return 1;
    }

    sprintf(command, "nano %s", argv[1]);
    system(command);

    lock.l_type = F_UNLCK;
    if (fcntl(fileDescriptor, F_SETLK, &lock) == -1) {
        perror("failed to lock file");
        return 1;
    }

    close(fileDescriptor);
    return 0;
}
