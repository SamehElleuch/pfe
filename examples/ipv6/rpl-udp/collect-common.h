

#ifndef __COLLECT_COMMON_H__
#define __COLLECT_COMMON_H__

#include "contiki.h"
#include "net/rime/rimeaddr.h"

void collect_common_net_init(void);
void collect_common_net_print(void);
void collect_common_set_sink(void);
void collect_common_send(void);
void collect_common_recv(const rimeaddr_t *originator, uint8_t seqno,
                         uint8_t hops,
                         uint8_t *payload,
                         uint16_t payload_len);
void collect_common_set_send_active(int active);

PROCESS_NAME(collect_common_process);

#endif /* __COLLECT_COMMON_H__ */
