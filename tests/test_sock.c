#define _POSIX_C_SOURCE 200809L
#undef NDEBUG
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "msg.h"
#include "sock.h"

int main(void)
{
    char path[64];
    snprintf(path, sizeof path, "/tmp/test_sock.%ld", (long)getpid());

    int lfd = sock_listen(path, 4);
    assert(lfd >= 0);

    /* Connect completes against the backlog, so one process can be both ends. */
    int cfd = sock_connect(path);
    assert(cfd >= 0);
    int sfd = sock_accept(lfd);
    assert(sfd >= 0);

    /* Sensor -> hub: readings then a heartbeat, boundaries preserved. */
    for (uint32_t i = 0; i < 3; i++) {
        struct msg m;
        memset(&m, 0, sizeof m);
        m.type = MSG_READING;
        m.u.reading.sensor_id = 2;
        m.u.reading.seq = i;
        m.u.reading.timestamp_ns = INT64_C(1700000000000000000) + i;
        m.u.reading.value = i * 1.5;
        assert(sock_send(cfd, &m) == 0);
    }
    struct msg hb;
    memset(&hb, 0, sizeof hb);
    hb.type = MSG_HEARTBEAT;
    hb.u.heartbeat.pid = (int32_t)getpid();
    hb.u.heartbeat.reading_count = 3;
    assert(sock_send(cfd, &hb) == 0);

    struct msg got;
    for (uint32_t i = 0; i < 3; i++) {
        assert(sock_recv(sfd, &got) == 0);
        assert(got.type == MSG_READING);
        assert(got.u.reading.sensor_id == 2);
        assert(got.u.reading.seq == i);
        assert(got.u.reading.value == i * 1.5);
    }
    assert(sock_recv(sfd, &got) == 0);
    assert(got.type == MSG_HEARTBEAT);
    assert(got.u.heartbeat.pid == (int32_t)getpid());
    assert(got.u.heartbeat.reading_count == 3);

    /* Hub -> sensor works too. */
    assert(sock_send(sfd, &hb) == 0);
    assert(sock_recv(cfd, &got) == 0);
    assert(got.type == MSG_HEARTBEAT);

    /* A wrong-sized packet is an error, and the connection stays usable. */
    uint32_t small = MSG_READING;
    assert(send(cfd, &small, sizeof small, 0) == (ssize_t)sizeof small);
    assert(sock_recv(sfd, &got) == -EBADMSG);
    char big[sizeof(struct msg) + 8] = { 0 };
    assert(send(cfd, big, sizeof big, 0) == (ssize_t)sizeof big);
    assert(sock_recv(sfd, &got) == -EMSGSIZE);
    assert(sock_send(cfd, &hb) == 0);
    assert(sock_recv(sfd, &got) == 0);

    /* Closed peer: recv and send report a disconnect instead of crashing
     * (send would raise SIGPIPE without MSG_NOSIGNAL). */
    assert(sock_close(cfd) == 0);
    assert(sock_recv(sfd, &got) == SOCK_DISCONNECTED);
    assert(sock_send(sfd, &hb) == SOCK_DISCONNECTED);

    /* Error paths return -errno. */
    assert(sock_connect("/tmp/test_sock.does-not-exist") == -ENOENT);
    char long_path[200];
    memset(long_path, 'x', sizeof long_path - 1);
    long_path[sizeof long_path - 1] = '\0';
    assert(sock_listen(long_path, 1) == -ENAMETOOLONG);

    assert(sock_close(sfd) == 0);
    assert(sock_close(lfd) == 0);
    assert(unlink(path) == 0);

    printf("test_sock: all asserts passed\n");
    return 0;
}
