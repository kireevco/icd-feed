#ifndef __SERIALD_H
#define __SERIALD_H

#define DPRINTF(format, ...) fprintf(stderr, "%s(%d): " format, __func__, __LINE__, ## __VA_ARGS__)

void fatal(const char *format, ...);

#endif /* __SERIALD_H */
