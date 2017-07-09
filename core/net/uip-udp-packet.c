

#include "contiki-conf.h"
#include "net/uip-debug.h"
extern uint16_t uip_slen;

#include "net/uip-udp-packet.h"

#include <string.h>

/*---------------------------------------------------------------------------*/
void
uip_udp_packet_send(struct uip_udp_conn *c, const void *data, int len)
{
#if UIP_UDP
  if(data != NULL) {
    uip_udp_conn = c;
    uip_slen = len;
    memcpy(&uip_buf[UIP_LLH_LEN + UIP_IPUDPH_LEN], data,
           len > UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN?
           UIP_BUFSIZE - UIP_LLH_LEN - UIP_IPUDPH_LEN: len);
    uip_process(UIP_UDP_SEND_CONN);
#if UIP_CONF_IPV6
    tcpip_ipv6_output();
#else
    if(uip_len > 0) {
      tcpip_output();
    }
#endif
  }
  uip_slen = 0;
#endif /* UIP_UDP */
}
/*---------------------------------------------------------------------------*/
void
uip_udp_packet_sendto(struct uip_udp_conn *c, const void *data, int len,
		      const uip_ipaddr_t *toaddr, uint16_t toport)
{
  uip_ipaddr_t curaddr;
  uint16_t curport;
  //uip_debug_ipaddr_print(toaddr);
  if(toaddr != NULL) {
    /* Save current IP addr/port. */
    uip_ipaddr_copy(&curaddr, &c->ripaddr);
    curport = c->rport;
    //uip_debug_ipaddr_print(toaddr);
    /* Load new IP addr/port */
    uip_debug_ipaddr_print(&c->ripaddr);
    uip_ipaddr_copy(&c->ripaddr, toaddr);
    c->rport = toport;
    uip_udp_packet_send(c, data, len);

    /* Restore old IP addr/port */
    uip_ipaddr_copy(&c->ripaddr, &curaddr);
    c->rport = curport;
  }
}
/*---------------------------------------------------------------------------*/
