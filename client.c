#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "sockname.h"

int main(void) {
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket");
		return -1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_NAME);

	int ret = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("connect");
		return -1;
	}

	pid_t pid = getpid();
	printf("@@ client pid=%d\n", pid);

	char buf[] = "hello world\n";
	ssize_t size = write(fd, buf, sizeof(buf));
	if (size == -1) {
		perror("write");
		return -1;
	}

	size = read(fd, buf, sizeof(buf));
	printf("@@ echo: %s\n", buf);

	return 0;
}
