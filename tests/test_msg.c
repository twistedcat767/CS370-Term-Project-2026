#undef NDEBUG
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "msg.h"

int main(void)
{
    assert(offsetof(struct msg, type) == 0);
    assert(offsetof(struct msg, u) == 8);

    struct msg m;
    memset(&m, 0, sizeof m);
    assert(m.type == MSG_INVALID);

    m.type = MSG_READING;
    m.u.reading.sensor_id = 3;
    m.u.reading.seq = 41;
    m.u.reading.timestamp_ns = INT64_C(1700000000000000000);
    m.u.reading.value = 21.5;
    m.u.reading.flags = MSG_FLAG_SIMULATED;

    unsigned char wire[sizeof m];
    memcpy(wire, &m, sizeof m);
    struct msg r;
    memcpy(&r, wire, sizeof r);
    assert(r.type == MSG_READING);
    assert(r.u.reading.sensor_id == 3);
    assert(r.u.reading.seq == 41);
    assert(r.u.reading.value == 21.5);
    assert(r.u.reading.flags == MSG_FLAG_SIMULATED);

    printf("test_msg: all asserts passed\n");
    return 0;
}
