
#ifndef MongooseEssentials_H_
#define MongooseEssentials_H_

#include <stdbool.h>
#include <string.h>

#include "inc/hw_memmap.h"

#include "mongoose.h"

int mg_ssl_if_mbed_random(void *ctx, unsigned char *buf, size_t len);

int gettimeofday(struct timeval *tv, void *tzvp);

void mg_lwip_mgr_schedule_poll(struct mg_mgr *mgr);

#endif
