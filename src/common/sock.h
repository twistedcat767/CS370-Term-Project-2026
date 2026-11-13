#ifndef SOCK_H
#define SOCK_H

#include "msg.h"

/*
 * Helpers for Unix-domain SOCK_SEQPACKET sockets carrying struct msg.
 *
 * SOCK_SEQPACKET keeps message boundaries, so one sock_send() arrives as
 * exactly one sock_recv().
 *
 * Return values:
 *   >= 0               success (a file descriptor for listen/accept/connect)
 *   SOCK_DISCONNECTED  the peer closed the connection (send/recv only)
 *   < 0                -errno; the error has already been logged to stderr
 *
 * All descriptors are created close-on-exec, and sending never raises
 * SIGPIPE, so a closed peer cannot kill the process.
 */

#define SOCK_DISCONNECTED 1

/* Create a listening socket at path, removing a stale socket file first. */
int sock_listen(const char *path, int backlog);

/* Accept one connection on a listening socket. Retries on EINTR. */
int sock_accept(int listen_fd);

/* Connect to the listening socket at path. */
int sock_connect(const char *path);

/* Send one message. */
int sock_send(int fd, const struct msg *m);

/*
 * Receive one message into m. A packet that is not exactly sizeof(struct msg)
 * is consumed and reported as -EMSGSIZE (too long) or -EBADMSG (too short),
 * so the connection stays usable.
 */
int sock_recv(int fd, struct msg *m);

/* Close fd, logging any error. Returns 0 or -errno. */
int sock_close(int fd);

#endif /* SOCK_H */
