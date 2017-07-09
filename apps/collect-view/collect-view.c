

#include "contiki.h"
#include "net/rime/collect-neighbor.h"
#include "net/rime.h"
#include "net/rime/timesynch.h"
#include "collect-view.h"
#include "sys/node-id.h"
#include <string.h>
#include <stdio.h>
//struct collect_view_data_msg msgM;
static int Msg_send[taille_send]={0};
static uint8_t dest_send[taille_send]={0};
int NbMsg_forward[taille_forward];
uint16_t node_forward[taille_forward];
/*---------------------------------------------------------------------------*/
void
collect_view_construct_message(struct collect_view_data_msg *msg,
                               const rimeaddr_t *parent,
                               uint16_t parent_etx,
                               uint16_t current_rtmetric,
                               uint16_t num_neighbors,
                               uint16_t beacon_interval,
                               int number_send)
{
  static unsigned long last_cpu, last_lpm, last_transmit, last_listen;
  unsigned long cpu, lpm, transmit, listen;
 int i,j;

  msg->len = sizeof(struct collect_view_data_msg) / sizeof(uint16_t);
  msg->clock = clock_time();
#if TIMESYNCH_CONF_ENABLED
  msg->timesynch_time = timesynch_time();
#else /* TIMESYNCH_CONF_ENABLED */
  msg->timesynch_time = 0;
#endif /* TIMESYNCH_CONF_ENABLED */

  energest_flush();

  cpu = energest_type_time(ENERGEST_TYPE_CPU) - last_cpu;
  lpm = energest_type_time(ENERGEST_TYPE_LPM) - last_lpm;
  transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT) - last_transmit;
  listen = energest_type_time(ENERGEST_TYPE_LISTEN) - last_listen;

  /* Make sure that the values are within 16 bits. If they are larger,
     we scale them down to fit into 16 bits. */
  while(cpu >= 65536ul || lpm >= 65536ul ||
	transmit >= 65536ul || listen >= 65536ul) {
    cpu /= 2;
    lpm /= 2;
    transmit /= 2;
    listen /= 2;
  }

  msg->cpu = cpu;
  msg->lpm = lpm;
  msg->transmit = transmit;
  msg->listen = listen;

  last_cpu = energest_type_time(ENERGEST_TYPE_CPU);
  last_lpm = energest_type_time(ENERGEST_TYPE_LPM);
  last_transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT);
  last_listen = energest_type_time(ENERGEST_TYPE_LISTEN);

  memcpy(&msg->parent, &parent->u8[RIMEADDR_SIZE - 2], 2);
  msg->parent_etx = parent_etx;
  msg->current_rtmetric = current_rtmetric;
  msg->num_neighbors = num_neighbors;
  msg->beacon_interval = beacon_interval;
  msg->number_send=number_send;
  printf("the send number is %d \n",number_send);
  for (i=0; i<taille_send ;i++) {
                                   if (dest_send[i]==0){
							      
                                                              dest_send[i]=parent->u8[RIMEADDR_SIZE - 2];
                                                              msg->dest_send[i]=parent->u8[RIMEADDR_SIZE - 2];
                                                              Msg_send[i]++;
                                                              msg->nb_send[i]=Msg_send[i];
                                                              printf("I send %d packet to %u\n",msg->nb_send[i],msg->dest_send[i]);
                                                              break;
                                                            }
                                  if (dest_send[i]== parent->u8[RIMEADDR_SIZE - 2]){
                                                              		              
                                                                                      Msg_send[i]++;
                                                                                      msg->nb_send[i]=Msg_send[i];
                                                                                      msg->dest_send[i]=dest_send[i];
                                                                                printf("I send %d packet to %u\n",msg->nb_send[i],msg->dest_send[i]);
                                                                                           break;
                                                            }
  }
    
  for (j=0; j<taille_forward ;j++) {
                                     msg->nb_forward[j]=NbMsg_forward[j];
                                     msg->src_forward[j]=node_forward[j];
                                     if(msg->nb_forward[j]!=0) {
                                                                  printf("I was forwarded %d message(s) from %u \n",NbMsg_forward[j],node_forward[j]);
                                                               }
  }
  memset(msg->sensors, 0, sizeof(msg->sensors));
  collect_view_arch_read_sensors(msg);
}
/*---------------------------------------------------------------------------*/
