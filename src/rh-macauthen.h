/**
 * RahuNAS MAC address authentication
 * Author: Neutron Soutmun <neo.neutron@gmail.com>
 * Date:   2014-12-02
 */

#ifndef __RH_MACAUTHEN_H
#define __RH_MACAUTHEN_H

#include <arpa/inet.h>
#include <net/if.h>

#include <libmnl/libmnl.h>
#include <linux/netfilter.h>
#include <linux/netfilter/nfnetlink.h>
#include <linux/ip.h>

#ifndef aligned_be64
#define aligned_be64 u_int64_t __attribute__((aligned(8)))
#endif

#include <linux/netfilter/nfnetlink_queue.h>
#include <glib.h>

#include "rh-server.h"

#define NO_MAC(m) (m[0] == 0 && m[1] == 0 && m[2] == 0 && \
                   m[3] == 0 && m[4] == 0 && m[5] == 0)

#define MACAUTHEN_ELEM_RETRY_CLEARTIME  10
#define MACAUTHEN_ELEM_DELAY_CLEARTIME  60

typedef struct macauthen_source MACAuthenSource;
typedef struct macauthen_elem   MACAuthenElem;
typedef struct macauthen_key    MACAuthenKey;

struct macauthen_source {
  GSource src;
  GPollFD pfd;
  guint   src_id;
  struct mnl_socket *nl;
  unsigned int nl_portid;
  unsigned int nl_queue_num;
};

struct macauthen_key {
  uint8_t        mac[8];
  struct in_addr src;
};

struct macauthen_elem {
  uint8_t        mac[8];
  struct in_addr src;
  uint32_t       iface_idx;
  time_t         last;
  time_t         cleartime;
  RHVServer     *vs;
};

int macauthen_setup (RHMainServer *main_server);

MACAuthenElem *macauthen_add_elem (uint8_t *mac, unsigned long s_addr,
                                   uint32_t iface_idx, time_t cleartime);

#endif // __RH_MACAUTHEN_H