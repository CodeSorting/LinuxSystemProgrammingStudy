#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
    int fd;
    char *fifofile = "/tmp/fifo_test";
    char inputbuf[1024];
    char readbuf[1024];

    if (access(fifofile, F_OK) != 0) {
        if (mkfifo(fifofile, 0666) == -1) {
            fprintf(stderr, "mkfifo error: %m\n");
            exit(1);
        }
        printf("FIFO file(%s) create OK.\n", fifofile);
    }

    while (1) {
        /* Read from FIFO */
        fd = open(fifofile, O_RDONLY);
        read(fd, readbuf, sizeof(readbuf));
        close(fd);

        printf("read: %s", readbuf); 
        printf("input message: ");
        fgets(inputbuf, sizeof(inputbuf), stdin);

        /* Write to FIFO */
        fd = open(fifofile, O_WRONLY);
        write(fd, inputbuf, strlen(inputbuf)+1);
        close(fd);
    }
    return 0;
}
