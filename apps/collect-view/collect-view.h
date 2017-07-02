#ifndef COLLECT_VIEW_H
#define COLLECT_VIEW_H
#define taille_send 20
#define taille_forward 20

#include "contiki-conf.h"
#include "net/rime/rimeaddr.h"
#include "net/rime/collect.h"
#include "sys/node-id.h"
//int Msg_send[taille_send]={0};
//uint8_t dest_send[taille_send]={0};
struct collect_view_data_msg {
  uint16_t len;
  uint16_t clock;
  uint16_t timesynch_time;
  uint16_t cpu;
  uint16_t lpm;
  uint16_t transmit;
  uint16_t listen;
  uint16_t parent;
  uint16_t parent_etx;
  uint16_t current_rtmetric;
  uint16_t num_neighbors;
  uint16_t beacon_interval;
  uint8_t node_id;
  int nb_send[taille_send];
  uint8_t dest_send[taille_send];
  int nb_forward[taille_forward];
  uint8_t src_forward[taille_forward];
  int number_send;
 
  uint16_t sensors[10];
};

void collect_view_construct_message(struct collect_view_data_msg *msg,
                                    const rimeaddr_t *parent,
                                    uint16_t etx_to_parent,
                                    uint16_t current_rtmetric,
                                    uint16_t num_neighbors,
                                    uint16_t beacon_interval,
                                    int number_send);

void collect_view_arch_read_sensors(struct collect_view_data_msg *msg);

#endif /* COLLECT_VIEW_H */
