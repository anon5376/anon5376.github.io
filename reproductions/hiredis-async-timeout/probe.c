/* SPDX-License-Identifier: MIT */
#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <poll.h>
#include "async.h"
#include "adapters/poll.h"

enum mode { MODE_ENQUEUE, MODE_ENABLE, MODE_SHORTEN };
struct counts { int queued, timeouts, replies, unexpected; };

static void reply_cb(redisAsyncContext *ac, void *reply, void *privdata) {
    struct counts *c = (struct counts *)privdata;
    if (reply == NULL && ac->err == REDIS_ERR_TIMEOUT) c->timeouts++;
    else { if (reply != NULL) c->replies++; c->unexpected++; }
}

static long long now_us(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        perror("clock_gettime");
        exit(2);
    }
    return (long long)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

static int queue_ping(redisAsyncContext *ac, struct counts *counts) {
    if (redisAsyncCommand(ac, reply_cb, counts, "PING") != REDIS_OK) return 0;
    counts->queued++;
    return 1;
}

static int tick_until_drained(redisAsyncContext *ac) {
    long long start = now_us();
    while (ac->err == 0 && (!(ac->c.flags & REDIS_CONNECTED) || sdslen(ac->c.obuf)) &&
           now_us() - start < 2000000) {
        if (redisPollTick(ac, 0.005) < 0) return 0;
    }
    return ac->err == 0 && (ac->c.flags & REDIS_CONNECTED) && sdslen(ac->c.obuf) == 0;
}

int main(int argc, char **argv) {
    enum mode mode;
    struct sockaddr_in addr;
    struct timeval connect_timeout = {3, 0}, command_timeout;
    redisOptions options;
    redisAsyncContext *ac = NULL;
    struct counts counts = {0, 0, 0, 0};
    int listener = -1, peer = -1, rc = 2;
    unsigned short port;
    long long start, end, next_enqueue;
    size_t unsent = 0;

    if (argc != 2) goto usage;
    if (strcmp(argv[1], "--enqueue") == 0) mode = MODE_ENQUEUE;
    else if (strcmp(argv[1], "--enable") == 0) mode = MODE_ENABLE;
    else if (strcmp(argv[1], "--shorten") == 0) mode = MODE_SHORTEN;
    else goto usage;

    memset(&addr, 0, sizeof(addr));
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) { perror("socket"); goto done; }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */
    if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) != 0 || listen(listener, 1) != 0) {
        perror("bind/listen"); goto done;
    }
    { socklen_t len = sizeof(addr); if (getsockname(listener, (struct sockaddr *)&addr, &len) != 0) { perror("getsockname"); goto done; } }
    port = ntohs(addr.sin_port);

    memset(&options, 0, sizeof(options));
    REDIS_OPTIONS_SET_TCP(&options, "127.0.0.1", port);
    options.connect_timeout = &connect_timeout;
    options.options = REDIS_OPT_NOAUTOFREE;
    if (mode == MODE_ENQUEUE) { command_timeout.tv_sec = 0; command_timeout.tv_usec = 200000; options.command_timeout = &command_timeout; }
    else if (mode == MODE_SHORTEN) { command_timeout.tv_sec = 3; command_timeout.tv_usec = 0; options.command_timeout = &command_timeout; }
    ac = redisAsyncConnectWithOptions(&options);
    if (ac == NULL || ac->err != 0) { fprintf(stderr, "connect setup: %s\n", ac ? ac->errstr : "allocation failed"); goto done; }
    if (redisPollAttach(ac) != REDIS_OK) { fprintf(stderr, "poll adapter attach failed\n"); goto done; }
    {
        struct pollfd pfd;
        int ready;
        pfd.fd = listener;
        pfd.events = POLLIN;
        pfd.revents = 0;
        ready = poll(&pfd, 1, 3000);
        if (ready <= 0 || !(pfd.revents & POLLIN)) {
            fprintf(stderr, "listener accept timed out or poll failed\n"); goto done;
        }
    }
    peer = accept(listener, NULL, NULL);
    if (peer < 0) { perror("accept"); goto done; }

    if (!queue_ping(ac, &counts) || !tick_until_drained(ac)) {
        fprintf(stderr, "initial PING did not connect and drain (err=%d)\n", ac->err); goto done;
    }
    if (mode == MODE_ENABLE || mode == MODE_SHORTEN) {
        command_timeout.tv_sec = 0; command_timeout.tv_usec = 200000;
        if (redisAsyncSetTimeout(ac, command_timeout) != REDIS_OK || !queue_ping(ac, &counts)) {
            fprintf(stderr, "runtime timeout setup or command enqueue failed\n"); goto done;
        }
        /* Hold write dispatch to isolate timer behavior from socket progress. */
        ac->ev.delWrite(ac->ev.data);
        start = now_us();
        while (ac->err == 0 && now_us() - start < 750000) redisPollTick(ac, 0.005);
    } else {
        if (!queue_ping(ac, &counts)) { fprintf(stderr, "PING enqueue failed\n"); goto done; }
        /* Prevent writes from changing the pending-command deadline. */
        ac->ev.delWrite(ac->ev.data);
        start = now_us();
        next_enqueue = start + 10000;
        while (ac->err == 0 && now_us() - start < 750000) {
            redisPollTick(ac, 0.005);
            if (ac->err == 0 && now_us() >= next_enqueue) {
                if (!queue_ping(ac, &counts)) { fprintf(stderr, "PING enqueue failed\n"); goto done; }
                ac->ev.delWrite(ac->ev.data);
                next_enqueue = now_us() + 10000;
            }
        }
    }
    end = now_us();
    unsent = sdslen(ac->c.obuf);
    printf("mode=%s elapsed_us=%lld err=%d queued=%d timeouts=%d replies=%d unexpected=%d unsent_bytes=%zu\n",
           argv[1], end - start, ac->err, counts.queued, counts.timeouts,
           counts.replies, counts.unexpected, unsent);
    rc = (ac->err == REDIS_ERR_TIMEOUT && counts.queued >= 2 &&
          counts.timeouts == counts.queued && counts.replies == 0 &&
          counts.unexpected == 0 && unsent > 0) ? 0 : 1;
    goto done;
usage:
    fprintf(stderr, "usage: %s --enqueue|--enable|--shorten\n", argv[0]);
    return 2;
done:
    if (ac != NULL) redisAsyncFree(ac);
    if (peer >= 0) close(peer);
    if (listener >= 0) close(listener);
    return rc;
}
