#ifndef __SERIALD_UBUS_H
#define __SERIALD_UBUS_H

extern struct ubus_context *ubus_ctx;

int seriald_ubus_init(const char *path);
void seriald_ubus_done(void);
void *seriald_ubus_loop(void *unused);

#endif /* __SERIALD_UBUS_H */
