#ifndef __SERIALD_UBUS_H
#define __SERIALD_UBUS_H

int seriald_ubus_connect(const char *path);
void seriald_ubus_disconnect(void);
void seriald_ubus_send_event(const char *msg);

#endif /* __SERIALD_UBUS_H */
