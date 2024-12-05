// user space application to test our device driver.
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void sigint_handler(int sig) {
	printf("SIGINT caught.\n");
}

int main() {
    int fd, ret, newfd;
    char buf[32];
	struct sigaction sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = sigint_handler;
	ret = sigaction(SIGINT, &sa, NULL);
    if(ret < 0) {
        perror("sigaction() failed");
        _exit(1);
    }

    fd = open("/dev/pchar2", O_WRONLY);
    if(fd < 0) {
        perror("open() failed");
        _exit(1);
    }
    strcpy(buf, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
        perror("3rd write() error");
    printf("write() returned: %d\n", ret);

    strcpy(buf, "1234567890");
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
        perror("3rd write() error");
    printf("write() returned: %d\n", ret);

    strcpy(buf, "+-*/");
    ret = write(fd, buf, strlen(buf));
    if(ret < 0)
        perror("3rd write() error");
    printf("write() returned: %d\n", ret);
    close(fd);

    return 0;
}
