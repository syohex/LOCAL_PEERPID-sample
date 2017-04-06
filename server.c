#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "sockname.h"

int main(void) {
	unlink(SOCKET_NAME);

	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd == -1) {
		perror("socket");
		return -1;
	}

	int enabled = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(int));
	if (ret == -1) {
		perror("setsockopt");
		return -1;
	}

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SOCKET_NAME);

	ret = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1) {
		perror("bind");
		return -1;
	}

	ret = listen(fd, 2);
	if (ret == -1) {
		perror("listen");
		return -1;
	}

	char buf[1024];
	while (1) {
		int client = accept(fd, NULL, NULL);
		if (client == -1) {
			perror("accept");
			continue;
		}

		pid_t pid;
		socklen_t pid_size = sizeof(pid);
		ret = getsockopt(client, SOL_LOCAL, LOCAL_PEERPID, &pid, &pid_size);
		if (ret == -1) {
			perror("getsockopt");
			return -1;
		}

		printf("@@ client pid=%d\n", pid);

		ssize_t size = read(client, buf, sizeof(buf));
		if (size == -1) {
			perror("read");
			continue;
		} else if (size == 0) {
			printf("finish\n");
			close(client);
		} else {
			write(client, buf, size);
		}
	}

	return 0;
}
