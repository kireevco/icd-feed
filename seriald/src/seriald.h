#ifndef __SERIALD_H
#define __SERIALD_H

#include <pthread.h>

#define DPRINTF(format, ...) fprintf(stderr, "%s(%d): " format, __func__, __LINE__, ## __VA_ARGS__)

#ifndef TTY_Q_SZ
#define TTY_Q_SZ 1024
#endif

struct tty_q {
	int len;
	char buff[TTY_Q_SZ];
} tty_q;

extern struct tty_q tty_q;
extern pthread_mutex_t write_q_mutex;
void fatal(const char *format, ...);

#endif /* __SERIALD_H */
