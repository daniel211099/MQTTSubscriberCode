#include "MongooseEssentials.h"

int mg_ssl_if_mbed_random(void *ctx, unsigned char *buf, size_t len){
    for(int i = 0; i < len; i++){
        buf[i] = rand() % (255 + 1);
    }
    return 0;
}

int gettimeofday(struct timeval *tv, void *tzvp) {
  tv->tv_sec = time(NULL);
  tv->tv_usec = 0;
  return 0;
}

void mg_lwip_mgr_schedule_poll(struct mg_mgr *mgr) {
}
