#define _GNU_SOURCE /* accept4, SOCK_CLOEXEC, MSG_NOSIGNAL */

#include "sock.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

static int log_err(const char *func, const char *call, const char *detail, int err)
{
    if (detail != NULL)
        fprintf(stderr, "%s: %s(%s): %s\n", func, call, detail, strerror(err));
    else
        fprintf(stderr, "%s: %s: %s\n", func, call, strerror(err));
    return -err;
}

static int fill_addr(const char *func, const char *path, struct sockaddr_un *addr)
{
    if (path == NULL || path[0] == '\0')
        return log_err(func, "path", "(empty)", EINVAL);

    size_t n = strlen(path);
    memset(addr, 0, sizeof *addr);
    if (n >= sizeof addr->sun_path)
        return log_err(func, "path", path, ENAMETOOLONG);

    addr->sun_family = AF_UNIX;
    memcpy(addr->sun_path, path, n + 1);
    return 0;
}

int sock_listen(const char *path, int backlog)
{
    struct sockaddr_un addr;
    int rc = fill_addr(__func__, path, &addr);
    if (rc < 0)
        return rc;

    int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0)
        return log_err(__func__, "socket", NULL, errno);

    if (unlink(path) < 0 && errno != ENOENT) {
        rc = log_err(__func__, "unlink", path, errno);
        goto cleanup;
    }
    if (bind(fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
        rc = log_err(__func__, "bind", path, errno);
        goto cleanup;
    }
    if (listen(fd, backlog) < 0) {
        rc = log_err(__func__, "listen", path, errno);
        goto cleanup_unlink;
    }
    return fd;

cleanup_unlink:
    if (unlink(path) < 0)
        log_err(__func__, "unlink", path, errno);
cleanup:
    sock_close(fd);
    return rc;
}

int sock_accept(int listen_fd)
{
    int fd;
    do {
        fd = accept4(listen_fd, NULL, NULL, SOCK_CLOEXEC);
    } while (fd < 0 && errno == EINTR);

    if (fd < 0)
        return log_err(__func__, "accept4", NULL, errno);
    return fd;
}

int sock_connect(const char *path)
{
    struct sockaddr_un addr;
    int rc = fill_addr(__func__, path, &addr);
    if (rc < 0)
        return rc;

    int fd = socket(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC, 0);
    if (fd < 0)
        return log_err(__func__, "socket", NULL, errno);

    /* An interrupted connect may still complete in the background, so
     * EINTR is reported rather than retried. */
    if (connect(fd, (struct sockaddr *)&addr, sizeof addr) < 0) {
        rc = log_err(__func__, "connect", path, errno);
        sock_close(fd);
        return rc;
    }
    return fd;
}

int sock_send(int fd, const struct msg *m)
{
    ssize_t n;
    do {
        n = send(fd, m, sizeof *m, MSG_NOSIGNAL);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EPIPE || errno == ECONNRESET)
            return SOCK_DISCONNECTED;
        return log_err(__func__, "send", NULL, errno);
    }
    /* SEQPACKET sends are all-or-nothing; anything else is a kernel bug. */
    if ((size_t)n != sizeof *m)
        return log_err(__func__, "send", "short write", EIO);
    return 0;
}

int sock_recv(int fd, struct msg *m)
{
    struct iovec iov = { .iov_base = m, .iov_len = sizeof *m };
    struct msghdr mh = { .msg_iov = &iov, .msg_iovlen = 1 };
    ssize_t n;
    do {
        n = recvmsg(fd, &mh, 0);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == ECONNRESET)
            return SOCK_DISCONNECTED;
        return log_err(__func__, "recvmsg", NULL, errno);
    }
    if (n == 0)
        return SOCK_DISCONNECTED;
    if (mh.msg_flags & MSG_TRUNC)
        return log_err(__func__, "recvmsg", "message too long", EMSGSIZE);
    if ((size_t)n != sizeof *m)
        return log_err(__func__, "recvmsg", "message too short", EBADMSG);
    return 0;
}

int sock_close(int fd)
{
    /* On Linux the fd is released even when close() fails, so never retry. */
    if (close(fd) < 0)
        return log_err(__func__, "close", NULL, errno);
    return 0;
}
