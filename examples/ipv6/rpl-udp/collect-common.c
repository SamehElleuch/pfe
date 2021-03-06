
#include "contiki.h"
#include "lib/random.h"
#include "net/netstack.h"
#include "dev/serial-line.h"
#include "dev/leds.h"
#include "collect-common.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static unsigned long time_offset;
static int send_active = 1;

#ifndef PERIOD
#define PERIOD 60
#endif
#define RANDWAIT (PERIOD)

/*---------------------------------------------------------------------------*/
PROCESS(collect_common_process, "collect common process");
AUTOSTART_PROCESSES(&collect_common_process);
/*---------------------------------------------------------------------------*/
static unsigned long
get_time(void)
{
  return clock_seconds() + time_offset;
}
/*---------------------------------------------------------------------------*/
static unsigned long
strtolong(const char *data) {
  unsigned long value = 0;
  int i;
  for(i = 0; i < 10 && isdigit(data[i]); i++) {
    value = value * 10 + data[i] - '0';
  }
  return value;
}
/*---------------------------------------------------------------------------*/
void
collect_common_set_send_active(int active)
{
  send_active = active;
}
/*---------------------------------------------------------------------------*/
void
collect_common_recv(const rimeaddr_t *originator, uint8_t seqno, uint8_t hops,
                    uint8_t *payload, uint16_t payload_len)
{
  unsigned long time;
  uint16_t data;
  int i;

  printf("%u", 8 + payload_len / 2);
  /* Timestamp. Ignore time synch for now. */
  time = get_time();
  printf(" %lu %lu 0", ((time >> 16) & 0xffff), time & 0xffff);
  /* Ignore latency for now */
  printf(" %u %u %u %u",
         originator->u8[0] + (originator->u8[1] << 8), seqno, hops, 0);
  for(i = 0; i < payload_len / 2; i++) {
    memcpy(&data, payload, sizeof(data));
    payload += sizeof(data);
    printf(" %u", data);
  }
  printf("\n");
  leds_blink();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(collect_common_process, ev, data)
{
  static struct etimer period_timer, wait_timer;
  PROCESS_BEGIN();

  collect_common_net_init();

  /* Send a packet every 60-62 seconds. */
  etimer_set(&period_timer, CLOCK_SECOND * PERIOD);
  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == serial_line_event_message) {
      char *line;
      line = (char *)data;
      if(strncmp(line, "collect", 7) == 0 ||
         strncmp(line, "gw", 2) == 0) {
        collect_common_set_sink();
      } else if(strncmp(line, "net", 3) == 0) {
        collect_common_net_print();
      } else if(strncmp(line, "time ", 5) == 0) {
        unsigned long tmp;
        line += 6;
        while(*line == ' ') {
          line++;
        }
        tmp = strtolong(line);
        time_offset = clock_seconds() - tmp;
        printf("Time offset set to %lu\n", time_offset);
      } else if(strncmp(line, "mac ", 4) == 0) {
        line +=4;
        while(*line == ' ') {
          line++;
        }
        if(*line == '0') {
          NETSTACK_RDC.off(1);
          printf("mac: turned MAC off (keeping radio on): %s\n",
                 NETSTACK_RDC.name);
        } else {
          NETSTACK_RDC.on();
          printf("mac: turned MAC on: %s\n", NETSTACK_RDC.name);
        }

      } else if(strncmp(line, "~K", 2) == 0 ||
                strncmp(line, "killall", 7) == 0) {
        /* Ignore stop commands */
      } else {
        printf("unhandled command: %s\n", line);
      }
    }
    if(ev == PROCESS_EVENT_TIMER) {
      if(data == &period_timer) {
        etimer_reset(&period_timer);
        etimer_set(&wait_timer, random_rand() % (CLOCK_SECOND * RANDWAIT));
      } else if(data == &wait_timer) {
        printf("I prepare to send \n");
        if(send_active) {
          /* Time to send the data */
          printf("I send \n");
          collect_common_send();
         
        }
         printf("I finish \n");
      }
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
