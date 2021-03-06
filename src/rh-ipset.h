/**
 * RahuNAS ipset implementation header
 * Author: Neutron Soutmun <neo.neutron@gmail.com>
 * Date:   2008-08-20
 */

#ifndef __RH_IPSET_H
#define __RH_IPSET_H

#include "rahunasd.h"
#include <linux/if_ether.h>
#include <ipset/ip_set.h>
#include <ipset/ip_set_rahunas.h>

#define GETSOCK_TRIES 5
#define LIST_TRIES 5

extern struct set **set_list;
extern ip_set_id_t max_sets;

/* The view of an ipset in userspace */
struct set {
  char name[IP_SET_MAXNAMELEN];   /* Name of the set */
  ip_set_id_t id;       /* Unique set id */
  ip_set_id_t index;      /* Array index */
  unsigned int ref;       /* References in kernel */
  struct settype *settype;    /* Pointer to set type functions */
};

struct processing_set {
  RHVServer *vs;
  void *list;
};


enum rh_adt_result {
  RH_IS_IN_SET,
  RH_IS_NOT_IN_SET,
};

/* Commands */
enum set_commands {
  CMD_NONE,
  CMD_CREATE,   /* -N */
  CMD_DESTROY,    /* -X */
  CMD_FLUSH,    /* -F */
  CMD_RENAME,   /* -E */
  CMD_SWAP,   /* -W */
  CMD_LIST,   /* -L */
  CMD_SAVE,   /* -S */
  CMD_RESTORE,    /* -R */
  CMD_ADD,    /* -A */
  CMD_DEL,    /* -D */
  CMD_TEST,   /* -T */
  CMD_BIND,   /* -B */
  CMD_UNBIND,   /* -U */
  CMD_HELP,   /* -H */
  CMD_VERSION,    /* -V */
  NUMBER_OF_CMD = CMD_VERSION,
  /* Internal commands */
  CMD_MAX_SETS,
  CMD_LIST_SIZE,
  CMD_SAVE_SIZE,
  CMD_ADT_GET,
};

enum exittype {
  OTHER_PROBLEM = 1,
  PARAMETER_PROBLEM,
  VERSION_PROBLEM
};

int kernel_getsocket(void);
int wrapped_getsockopt(void *data, socklen_t *size);
int wrapped_setsockopt(void *data, socklen_t size);
void kernel_getfrom(void *data, socklen_t * size);
int kernel_sendto_handleerrno(unsigned int op, void *data, socklen_t size);
void kernel_sendto(void *data, size_t size);
int kernel_getfrom_handleerrno(void *data, socklen_t * size);
struct set *set_adt_get(const char *name);
int set_adtip(struct set *rahunas_set, const char *adtip, const char *adtmac, 
              unsigned int op);

int set_adtip_nb(struct set *rahunas_set, ip_set_ip_t *adtip, 
                 unsigned char adtmac[ETH_ALEN], unsigned int op);

int set_ipiphash_adtip(struct set *rahunas_set, const char *ip,
                       const char *ip1, unsigned int op);
int set_ipiphash_adtip_nb(struct set *rahunas_set, ip_set_ip_t *ip,
                          ip_set_ip_t *ip1, unsigned int op);

void set_flush(const char *name);

size_t load_set_list(RHVServer *vs, const char name[IP_SET_MAXNAMELEN],
          ip_set_id_t *idx,
          unsigned int op, unsigned int cmd);

int get_header_from_set (RHVServer *vs);

int walk_through_set (int (*callback)(void *), RHVServer *vs);

void parse_ip(const char *str, ip_set_ip_t *ip);
void parse_mac(const char *mac, unsigned char *ethernet);
char *ip_tostring(ip_set_ip_t ip);
char *mac_tostring(unsigned char macaddress[ETH_ALEN]);

struct ip_set_req_rahunas *get_data_from_set (void *data, unsigned int id,
                                              const struct rahunas_map *map);

#define BITSPERBYTE (8*sizeof(char))
#define ID2BYTE(id) ((id)/BITSPERBYTE)
#define ID2MASK(id) (1 << ((id)%BITSPERBYTE))
#define test_bit(id, heap)  ((((char *)(heap))[ID2BYTE(id)] & ID2MASK(id)) != 0)

#endif //__RH_IPSET_H
