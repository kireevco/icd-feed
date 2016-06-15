#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <limits.h>

#define _GNU_SOURCE
#include <getopt.h>

#include "seriald.h"
#include "strutils.h"
#include "fdio.h"
#include "term.h"
#include "ubus.h"
#include "ubus_loop.h"

/* TODO: open source */
/* TODO: support base64 for wrapping the data */

#define STO STDOUT_FILENO
#define STI STDIN_FILENO

#define TTY_RD_SZ 256

char buff_rd_line[TTY_RD_SZ+1] = "";

int tty_fd;

#define TTY_WRITE_SZ_DIV 10
#define TTY_WRITE_SZ_MIN 8

int tty_write_sz;

#define set_tty_write_sz(baud) \
	do { \
		tty_write_sz = (baud) / TTY_WRITE_SZ_DIV; \
		if (tty_write_sz < TTY_WRITE_SZ_MIN) tty_write_sz = TTY_WRITE_SZ_MIN; \
	} while (0)

struct tty_q tty_q;

pthread_mutex_t write_q_mutex;

int sig_exit = 0;

/* parity modes names */
const char *parity_str[] = {
	[P_NONE] = "none",
	[P_EVEN] = "even",
	[P_ODD] = "odd",
	[P_MARK] = "mark",
	[P_SPACE] = "space",
};

/* flow control modes names */
const char *flow_str[] = {
	[FC_NONE] = "none",
	[FC_RTSCTS] = "RTS/CTS",
	[FC_XONXOFF] = "xon/xoff",
	[FC_OTHER] = "other",
};

struct {
	char port[128];
	int baud;
	enum flowcntrl_e flow;
	enum parity_e parity;
	int databits;
	int stopbits;
	int noreset;
	char *socket;
} opts = {
	.port = "",
	.baud = 9600,
	.flow = FC_NONE,
	.parity = P_NONE,
	.databits = 8,
	.stopbits = 1,
	.noreset = 0,
	.socket = NULL, /* the library fall back to default socket when it is NULL */
};

static pthread_t uloop_tid;

static void show_usage(void);
static void parse_args(int argc, char *argv[]);
static void deadly_handler(int signum);
static void register_signal_handlers(void);
static void loop(void);
static void tty_read_line_parser(const int n, const char *buff_rd);
static void tty_read_line_cb(const char *line);
int main(int argc, char *argv[]);

static void show_usage()
{
	printf("usage: seriald [options] <TTY device>\n");
	printf("\n");
	printf("options:\n");
	printf("  -b <baudrate>\n");
	printf("    baudrate should be one of: 0, 50, 75, 110, 134, 150, 200, 300, 600,\n");
	printf("    1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400\n");
	printf("  -f s (=soft) | h (=hard) | n (=none)\n");
	printf("  -s <ubus socket> (no need to give if you use the default one)");
}

void fatal(const char *format, ...)
{
	char *s, buf[256];
	va_list args;
	int len;

	va_start(args, format);
	len = vsnprintf(buf, sizeof(buf), format, args);
	buf[sizeof(buf) - 1] = '\0';
	va_end(args);
	
	s = "\r\nFATAL: ";
	writen_ni(STO, s, strlen(s));
	writen_ni(STO, buf, len);
	s = "\r\n";
	writen_ni(STO, s, strlen(s));

	/* wait a bit for output to drain */
	sleep(1);

	exit(EXIT_FAILURE);
}

static void parse_args(int argc, char *argv[])
{
	int c;
	int r = 0;

	while ((c = getopt(argc, argv, "hf:b:s:")) != -1) {
		switch (c) {
			case 'f':
				switch (optarg[0]) {
					case 'X':
					case 'x':
						opts.flow = FC_XONXOFF;
						break;
					case 'H':
					case 'h':
						opts.flow = FC_RTSCTS;
						break;
					case 'N':
					case 'n':
						opts.flow = FC_NONE;
						break;
					default:
						DPRINTF("Invalid flow control: %c\n", optarg[0]);
						r = -1;
						break;
				}
				break;
			case 'b':
				opts.baud = atoi(optarg);
				if (opts.baud == 0 || !term_baud_ok(opts.baud)) {
					DPRINTF("Invalid baud rate: %d\n", opts.baud);
					r = -1;
				}
				break;
			case 's':
				opts.socket = optarg;
				break;
			case 'h':
				r = 1;
				break;
			default:
				r = -1;
				break;
		}
	}

	if (r) {
		show_usage();
		exit((r > 0) ? EXIT_SUCCESS : EXIT_FAILURE);
	}

	if ((argc - optind) < 1) {
		DPRINTF("No port given\n");
		show_usage();
		exit(EXIT_FAILURE);
	}

	strncpy(opts.port, argv[optind], sizeof(opts.port) - 1);
	opts.port[sizeof(opts.port) - 1] = '\0';
}

static void deadly_handler(int signum)
{
	DPRINTF("seriald is signaled with TERM\n");
	if (!sig_exit) {
		seriald_ubus_loop_stop();
		sig_exit = 1;
	}
}

static void register_signal_handlers(void)
{
	struct sigaction exit_action, ign_action;

	/* Set up the structure to specify the exit action. */
	exit_action.sa_handler = deadly_handler;
	sigemptyset (&exit_action.sa_mask);
	exit_action.sa_flags = 0;

	/* Set up the structure to specify the ignore action. */
	ign_action.sa_handler = SIG_IGN;
	sigemptyset (&ign_action.sa_mask);
	ign_action.sa_flags = 0;

	sigaction(SIGTERM, &exit_action, NULL);

	sigaction(SIGALRM, &ign_action, NULL);
	sigaction(SIGHUP, &ign_action, NULL);
	sigaction(SIGINT, &ign_action, NULL);
	sigaction(SIGPIPE, &ign_action, NULL);
	sigaction(SIGQUIT, &ign_action, NULL);
	sigaction(SIGUSR1, &ign_action, NULL);
	sigaction(SIGUSR2, &ign_action, NULL);
}

static void loop(void)
{
	fd_set rdset, wrset;
	int r;
	int n;
	char buff_rd[TTY_RD_SZ];
	int write_sz;

	tty_q.len = 0;

	while (!sig_exit) {
		FD_ZERO(&rdset);
		FD_ZERO(&wrset);
		FD_SET(tty_fd, &rdset);

		pthread_mutex_lock(&write_q_mutex);
		if (tty_q.len) FD_SET(tty_fd, &wrset);
		pthread_mutex_unlock(&write_q_mutex);

		r = select(tty_fd + 1, &rdset, &wrset, NULL, NULL);
		if (r < 0)  {
			if (errno == EINTR){
				continue;
			} else {
				fatal("select failed: %d : %s", errno, strerror(errno));
			}
		}

		if (FD_ISSET(tty_fd, &rdset)) {
			/* read from port */
			do {
				n = read(tty_fd, &buff_rd, sizeof(buff_rd));
			} while (n < 0 && errno == EINTR);
			if (n == 0) {
				fatal("term closed");
			} else if (n < 0) {
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					fatal("read from term failed: %s", strerror(errno));
			} else {
				tty_read_line_parser(n, buff_rd);
			}
		}

		if (FD_ISSET(tty_fd, &wrset)) {
			/* write to port */
			pthread_mutex_lock(&write_q_mutex);
			write_sz = (tty_q.len < tty_write_sz) ? tty_q.len : tty_write_sz;
			do {
				n = write(tty_fd, tty_q.buff, write_sz);
			} while (n < 0 && errno == EINTR);
			if (n <= 0) fatal("write to term failed: %s", strerror(errno));
			memmove(tty_q.buff, tty_q.buff + n, tty_q.len - n);
			tty_q.len -= n;
			pthread_mutex_unlock(&write_q_mutex);
		}
	}
}

static void tty_read_line_parser(const int n, const char *buff_rd)
{
	const char *p;
	const char *pp;
	char buff[TTY_RD_SZ+1] = "";

	/* buff_rd isn't null-terminated */
	strncat(buff, buff_rd, n);

	pp = buff;

	while ((p = strchr(pp, '\n'))) {
		if (strlen(buff_rd_line) + p - pp > TTY_RD_SZ) {
			tty_read_line_cb(buff_rd_line);
			*buff_rd_line = '\0';
		}

		strncat(buff_rd_line, pp, p - pp);
		strchrdel(buff_rd_line, '\r');
		tty_read_line_cb(buff_rd_line);
		*buff_rd_line = '\0';

		pp = p + 1;
	}

	if (pp) {
		if (strlen(buff_rd_line) + n - (pp - buff) > TTY_RD_SZ) {
			tty_read_line_cb(buff_rd_line);
			*buff_rd_line = '\0';
		}
		strncat(buff_rd_line, pp, n - (pp - buff));
		strchrdel(buff_rd_line, '\r');
	}
}

static void tty_read_line_cb(const char *line)
{
	int status;
	pid_t pid;
	char format[] = "{\"data\": \"%s\"}";
	char buff[sizeof(format)+TTY_RD_SZ];

	sprintf(buff, format, line);

	pid = fork();
	if (!pid) {
		if (opts.socket) {
			execl("/bin/ubus", "ubus", "-s", opts.socket,
					"send", "serial", buff, (char *) NULL);
		} else {
			execl("/bin/ubus", "ubus", "send", "serial", buff, (char *) NULL);
		}
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		waitpid(-1, &status, WNOHANG);
	}
}

int main(int argc, char *argv[])
{
	int r;

	parse_args(argc, argv);
	register_signal_handlers();

	r = term_lib_init();
	if (r < 0) fatal("term_init failed: %s", term_strerror(term_errno, errno));

	tty_fd = open(opts.port, O_RDWR | O_NONBLOCK | O_NOCTTY);
	if (tty_fd < 0) fatal("cannot open %s: %s", opts.port, strerror(errno));

	r = term_set(tty_fd,
			1,              /* raw mode. */
			opts.baud,      /* baud rate. */
			opts.parity,    /* parity. */
			opts.databits,  /* data bits. */
			opts.stopbits,  /* stop bits. */
			opts.flow,      /* flow control. */
			1,              /* local or modem */
			!opts.noreset); /* hup-on-close. */
	if (r < 0) {
		fatal("failed to add device %s: %s",
				opts.port, term_strerror(term_errno, errno));
	}

	r = term_apply(tty_fd, 0);
	if (r < 0) {
		fatal("failed to config device %s: %s",
				opts.port, term_strerror(term_errno, errno));
	}

	set_tty_write_sz(term_get_baudrate(tty_fd, NULL));

	if (seriald_ubus_loop_init(opts.socket)) {
		DPRINTF("Failed to connect to ubus\n");
		return -EIO;
	}

	r = pthread_create(&uloop_tid, NULL, &seriald_ubus_loop, NULL);
	if (r) fatal("can't create thread for uloop: %s", strerror(r));

	loop();

	seriald_ubus_loop_done();

	return EXIT_SUCCESS;
}
