#ifndef MSG_H
#define MSG_H

#include <assert.h>
#include <stdint.h>

/*
 * Wire format for sensor -> hub messages.
 *
 * Every message is a fixed-size struct msg, so the hub can read exactly
 * sizeof(struct msg) bytes per message. Because it is no larger than 512
 * bytes (_POSIX_PIPE_BUF, the minimum PIPE_BUF any POSIX system allows),
 * a single write() of one message to a pipe or FIFO is atomic, even with
 * several sensors writing to the same pipe.
 *
 * Only fixed-width types are used, and padding is explicit. Senders zero
 * the struct before filling it in, so no uninitialized bytes go on the wire.
 */

enum msg_type {
    MSG_INVALID   = 0, /* zeroed memory is never a valid message */
    MSG_READING   = 1,
    MSG_HEARTBEAT = 2,
};

/* Bits for msg_reading.flags */
#define MSG_FLAG_SENSOR_ERROR UINT32_C(0x1) /* read failed; value is not meaningful */
#define MSG_FLAG_OUT_OF_RANGE UINT32_C(0x2) /* value outside the sensor's valid range */
#define MSG_FLAG_SIMULATED    UINT32_C(0x4) /* produced without real hardware */

struct msg_reading {
    uint32_t sensor_id;
    uint32_t seq;          /* starts at 0, +1 per reading, per sensor */
    int64_t  timestamp_ns; /* CLOCK_REALTIME, nanoseconds since the epoch */
    double   value;
    uint32_t flags;        /* MSG_FLAG_* */
    uint32_t _pad;
};

struct msg_heartbeat {
    int32_t  pid;           /* sender's getpid() */
    uint32_t reading_count; /* readings sent since the sender started */
};

struct msg {
    uint32_t type; /* enum msg_type */
    uint32_t _pad;
    union {
        struct msg_reading   reading;
        struct msg_heartbeat heartbeat;
    } u;
};

static_assert(sizeof(struct msg_reading) == 32, "msg_reading layout changed");
static_assert(sizeof(struct msg_heartbeat) == 8, "msg_heartbeat layout changed");
static_assert(sizeof(struct msg) == 40, "msg layout changed");
static_assert(sizeof(struct msg) <= 512, "msg must fit in one atomic pipe write");

#endif /* MSG_H */
