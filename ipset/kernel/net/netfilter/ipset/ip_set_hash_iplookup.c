/* Copyright (C) 2012 Neutron Soutmun <neo.neutron@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* Kernel module implementing an IP set type: the hash:iplookup type
 * This module derived from IP set type: the hash:ip,port,ip
 */

#include <linux/jhash.h>
#include <linux/module.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/errno.h>
#include <linux/random.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/netlink.h>
#include <net/tcp.h>

#include <linux/netfilter.h>
#include <linux/netfilter/ipset/pfxlen.h>
#include <linux/netfilter/ipset/ip_set.h>
#include <linux/netfilter/ipset/ip_set_timeout.h>
#include <linux/netfilter/ipset/ip_set_hash.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Neutron Soutmun <neo.neutron@gmail.com>");
MODULE_DESCRIPTION("hash:iplookup type of IP sets");
MODULE_ALIAS("ip_set_hash:iplookup");

/* Type specific function prefix */
#define TYPE		hash_iplookup

static bool
hash_iplookup_same_set(const struct ip_set *a, const struct ip_set *b);

#define hash_iplookup4_same_set	hash_iplookup_same_set
#define hash_iplookup6_same_set	hash_iplookup_same_set

/* The type variant functions: IPv4 */

/* Member elements without timeout */
struct hash_iplookup4_elem {
	__be32 ip;
	__be32 ip2;
};

/* Member elements with timeout support */
struct hash_iplookup4_telem {
	__be32 ip;
	__be32 ip2;
	unsigned long timeout;
};

static inline bool
hash_iplookup4_data_equal(const struct hash_iplookup4_elem *ip1,
                          const struct hash_iplookup4_elem *ip2,
                          u32 *multi)
{
	return ip1->ip == ip2->ip;
}

static inline bool
hash_iplookup4_data_isnull(const struct hash_iplookup4_elem *elem)
{
	return elem->ip == 0;
}

static inline void
hash_iplookup4_data_copy(struct hash_iplookup4_elem *dst,
                         const struct hash_iplookup4_elem *src)
{
	memcpy(dst, src, sizeof(*dst));
}

static inline void
hash_iplookup4_data_zero_out(struct hash_iplookup4_elem *elem)
{
	elem->ip = 0;
}

static bool
hash_iplookup4_data_list(struct sk_buff *skb,
                         const struct hash_iplookup4_elem *data)
{
	NLA_PUT_IPADDR4(skb, IPSET_ATTR_IP,  data->ip);
	NLA_PUT_IPADDR4(skb, IPSET_ATTR_IP2, data->ip2);
	return 0;

nla_put_failure:
	return 1;
}

static bool
hash_iplookup4_data_tlist(struct sk_buff *skb,
                          const struct hash_iplookup4_elem *data)
{
	const struct hash_iplookup4_telem *tdata =
		(const struct hash_iplookup4_telem *)data;

	NLA_PUT_IPADDR4(skb, IPSET_ATTR_IP,  tdata->ip);
	NLA_PUT_IPADDR4(skb, IPSET_ATTR_IP2, tdata->ip2);
	NLA_PUT_NET32(skb, IPSET_ATTR_TIMEOUT,
	              htonl(ip_set_timeout_get(tdata->timeout)));

	return 0;

nla_put_failure:
	return 1;
}

#define PF		4
#define HOST_MASK	32
#include <linux/netfilter/ipset/ip_set_hash_iplookup.h>

static inline void
hash_iplookup4_data_next(struct ip_set_hash *h,
                         const struct hash_iplookup4_elem *d)
{
	h->next.ip = ntohl(d->ip);
}

static int
hash_iplookup4_kadt(struct ip_set *set, const struct sk_buff *skb,
                    const struct xt_action_param *par,
                    enum ipset_adt adt, const struct ip_set_adt_opt *opt)
{
	const struct ip_set_hash *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_iplookup4_elem data = { };

	ip4addrptr(skb, opt->flags & IPSET_DIM_ONE_SRC, &data.ip);
	ip4addrptr(skb, (opt->flags & IPSET_DIM_ONE_SRC) ? 0 : 1, &data.ip2);

	return adtfn(set, &data, opt_timeout(opt, h), opt->cmdflags);
}

static int
hash_iplookup4_uadt(struct ip_set *set, struct nlattr *tb[],
                    enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	const struct ip_set_hash *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_iplookup4_elem data = { };
	u32 ip, ip_to = 0;
	u32 timeout = h->timeout;
	int ret;

	if (unlikely(!tb[IPSET_ATTR_IP] ||
		     !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT)))
		return -IPSET_ERR_PROTOCOL;

	if (adt == IPSET_ADD && unlikely(!tb[IPSET_ATTR_IP2]))
		return -IPSET_ERR_PROTOCOL;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	ret = ip_set_get_ipaddr4(tb[IPSET_ATTR_IP], &data.ip);
	if (ret)
		return ret;

	if (adt == IPSET_ADD) {
		ret = ip_set_get_ipaddr4(tb[IPSET_ATTR_IP2], &data.ip2);
		if (ret)
			return ret;
	}

	if (tb[IPSET_ATTR_TIMEOUT]) {
		if (!with_timeout(h->timeout))
			return -IPSET_ERR_TIMEOUT;
		timeout = ip_set_timeout_uget(tb[IPSET_ATTR_TIMEOUT]);
	}

	if (adt == IPSET_TEST ||
	    !(tb[IPSET_ATTR_IP_TO] || tb[IPSET_ATTR_CIDR])) {
		ret = adtfn(set, &data, timeout, flags);
		return ip_set_eexist(ret, flags) ? 0 : ret;
	}

	ip = ntohl(data.ip);
	if (tb[IPSET_ATTR_IP_TO]) {
		ret = ip_set_get_hostipaddr4(tb[IPSET_ATTR_IP_TO], &ip_to);
		if (ret)
			return ret;
		if (ip > ip_to)
			swap(ip, ip_to);
	} else if (tb[IPSET_ATTR_CIDR]) {
		u8 cidr = nla_get_u8(tb[IPSET_ATTR_CIDR]);

		if (cidr > 32)
			return -IPSET_ERR_INVALID_CIDR;
		ip_set_mask_from_to(ip, ip_to, cidr);
	} else
		ip_to = ip;

	if (retried)
		ip = h->next.ip;
	for (; !before(ip_to, ip); ip++) {
		data.ip = htonl(ip);
		ret = adtfn(set, &data, timeout, flags);

		if (ret && !ip_set_eexist(ret, flags))
			return ret;
		else
			ret = 0;
	}
	return ret;
}

static bool
hash_iplookup_same_set(const struct ip_set *a, const struct ip_set *b)
{
	const struct ip_set_hash *x = a->data;
	const struct ip_set_hash *y = b->data;

	/* Resizing changes htable_bits, so we ignore it */
	return x->maxelem == y->maxelem &&
	       x->timeout == y->timeout;
}

/* The type variant functions: IPv6 */

struct hash_iplookup6_elem {
	union nf_inet_addr ip;
	union nf_inet_addr ip2;
};

struct hash_iplookup6_telem {
	union nf_inet_addr ip;
	union nf_inet_addr ip2;
	unsigned long timeout;
};

static inline bool
hash_iplookup6_data_equal(const struct hash_iplookup6_elem *ip1,
                          const struct hash_iplookup6_elem *ip2,
                          u32 *multi)
{
	return ipv6_addr_cmp(&ip1->ip.in6, &ip2->ip.in6) == 0;
}

static inline bool
hash_iplookup6_data_isnull(const struct hash_iplookup6_elem *elem)
{
	return ipv6_addr_any(&elem->ip.in6);
}

static inline void
hash_iplookup6_data_copy(struct hash_iplookup6_elem *dst,
                         const struct hash_iplookup6_elem *src)
{
	memcpy(dst, src, sizeof(*dst));
}

static inline void
hash_iplookup6_data_zero_out(struct hash_iplookup6_elem *elem)
{
	ipv6_addr_set(&elem->ip.in6, 0, 0, 0, 0);
}

static bool
hash_iplookup6_data_list(struct sk_buff *skb,
                         const struct hash_iplookup6_elem *data)
{
	NLA_PUT_IPADDR6(skb, IPSET_ATTR_IP,  &data->ip);
	NLA_PUT_IPADDR6(skb, IPSET_ATTR_IP2, &data->ip2);
	return 0;

nla_put_failure:
	return 1;
}

static bool
hash_iplookup6_data_tlist(struct sk_buff *skb,
                          const struct hash_iplookup6_elem *data)
{
	const struct hash_iplookup6_telem *tdata =
		(const struct hash_iplookup6_telem *)data;

	NLA_PUT_IPADDR6(skb, IPSET_ATTR_IP,  &tdata->ip);
	NLA_PUT_IPADDR6(skb, IPSET_ATTR_IP2, &tdata->ip2);
	NLA_PUT_NET32(skb, IPSET_ATTR_TIMEOUT,
	              htonl(ip_set_timeout_get(tdata->timeout)));
	return 0;

nla_put_failure:
	return 1;
}

#undef PF
#undef HOST_MASK

#define PF		6
#define HOST_MASK	128
#include <linux/netfilter/ipset/ip_set_hash_iplookup.h>

static inline void
hash_iplookup6_data_next(struct ip_set_hash *h,
                         const struct hash_iplookup6_elem *d)
{
	/* Do nothing */
}

static int
hash_iplookup6_kadt(struct ip_set *set, const struct sk_buff *skb,
                    const struct xt_action_param *par,
                    enum ipset_adt adt, const struct ip_set_adt_opt *opt)
{
	const struct ip_set_hash *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_iplookup6_elem data = { };

	ip6addrptr(skb, opt->flags & IPSET_DIM_ONE_SRC, &data.ip.in6);
	ip6addrptr(skb, (opt->flags & IPSET_DIM_ONE_SRC) ? 0 : 1,
	                 &data.ip2.in6);

	return adtfn(set, &data, opt_timeout(opt, h), opt->cmdflags);
}

static int
hash_iplookup6_uadt(struct ip_set *set, struct nlattr *tb[],
                    enum ipset_adt adt, u32 *lineno, u32 flags, bool retried)
{
	const struct ip_set_hash *h = set->data;
	ipset_adtfn adtfn = set->variant->adt[adt];
	struct hash_iplookup6_elem data = { };
	u32 timeout = h->timeout;
	int ret;

	if (unlikely(!tb[IPSET_ATTR_IP] ||
	             !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT) ||
	             tb[IPSET_ATTR_IP_TO] || tb[IPSET_ATTR_CIDR]))
		return -IPSET_ERR_PROTOCOL;

	if (adt == IPSET_ADD && unlikely(!tb[IPSET_ATTR_IP2]))
		return -IPSET_ERR_PROTOCOL;

	if (tb[IPSET_ATTR_LINENO])
		*lineno = nla_get_u32(tb[IPSET_ATTR_LINENO]);

	ret = ip_set_get_ipaddr6(tb[IPSET_ATTR_IP], &data.ip);
	if (ret)
		return ret;

	if (adt == IPSET_ADD) {
		ret = ip_set_get_ipaddr6(tb[IPSET_ATTR_IP2], &data.ip2);
		if (ret)
			return ret;
	}

	if (tb[IPSET_ATTR_TIMEOUT]) {
		if (!with_timeout(h->timeout))
			return -IPSET_ERR_TIMEOUT;
		timeout = ip_set_timeout_uget(tb[IPSET_ATTR_TIMEOUT]);
	}

	if (adt == IPSET_TEST) {
		ret = adtfn(set, &data, timeout, flags);
		return ip_set_eexist(ret, flags) ? 0 : ret;
	}

	ret = adtfn(set, &data, timeout, flags);
	if (ret && !ip_set_eexist(ret, flags))
		return ret;
	else
		return 0;
}

/* Create hash:iplookup type of sets */

static int
hash_iplookup_create(struct ip_set *set, struct nlattr *tb[], u32 flags)
{
	struct ip_set_hash *h;
	u32 hashsize = IPSET_DEFAULT_HASHSIZE, maxelem = IPSET_DEFAULT_MAXELEM;
	u8 hbits;

	if (!(set->family == NFPROTO_IPV4 || set->family == NFPROTO_IPV6))
		return -IPSET_ERR_INVALID_FAMILY;

	if (unlikely(!ip_set_optattr_netorder(tb, IPSET_ATTR_HASHSIZE) ||
	             !ip_set_optattr_netorder(tb, IPSET_ATTR_MAXELEM) ||
	             !ip_set_optattr_netorder(tb, IPSET_ATTR_TIMEOUT)))
		return -IPSET_ERR_PROTOCOL;

	if (tb[IPSET_ATTR_HASHSIZE]) {
		hashsize = ip_set_get_h32(tb[IPSET_ATTR_HASHSIZE]);
		if (hashsize < IPSET_MIMINAL_HASHSIZE)
			hashsize = IPSET_MIMINAL_HASHSIZE;
	}

	if (tb[IPSET_ATTR_MAXELEM])
		maxelem = ip_set_get_h32(tb[IPSET_ATTR_MAXELEM]);

	h = kzalloc(sizeof(*h), GFP_KERNEL);
	if (!h)
		return -ENOMEM;

	h->maxelem = maxelem;
	get_random_bytes(&h->initval, sizeof(h->initval));
	h->timeout = IPSET_NO_TIMEOUT;

	hbits = htable_bits(hashsize);
	h->table = ip_set_alloc(
			sizeof(struct htable) +
			jhash_size(hbits) * sizeof(struct hbucket));
	if (!h->table) {
		kfree(h);
		return -ENOMEM;
	}
	h->table->htable_bits = hbits;

	set->data = h;

	if (tb[IPSET_ATTR_TIMEOUT]) {
		h->timeout = ip_set_timeout_uget(tb[IPSET_ATTR_TIMEOUT]);

		set->variant = set->family == NFPROTO_IPV4 ?
			&hash_iplookup4_tvariant : &hash_iplookup6_tvariant;

		if (set->family == NFPROTO_IPV4)
			hash_iplookup4_gc_init(set);
		else
			hash_iplookup6_gc_init(set);
	} else {
		set->variant = set->family == NFPROTO_IPV4 ?
			&hash_iplookup4_variant : &hash_iplookup6_variant;
	}

	pr_debug("create %s hashsize %u (%u) maxelem %u: %p(%p)\n",
	         set->name, jhash_size(h->table->htable_bits),
	         h->table->htable_bits, h->maxelem, set->data, h->table);

	return 0;
}

static struct ip_set_type hash_iplookup_type __read_mostly = {
	.name          = "hash:iplookup",
	.protocol      = IPSET_PROTOCOL,
	.features      = IPSET_TYPE_IP,
	.dimension     = IPSET_DIM_ONE,
	.family        = NFPROTO_UNSPEC,
	.revision_min  = 0,
	.revision_max  = 1,	/* SCTP and UDPLITE support added */
	.create        = hash_iplookup_create,
	.create_policy = {
		[IPSET_ATTR_HASHSIZE] = { .type = NLA_U32 },
		[IPSET_ATTR_MAXELEM]  = { .type = NLA_U32 },
		[IPSET_ATTR_PROBES]   = { .type = NLA_U8 },
		[IPSET_ATTR_RESIZE]   = { .type = NLA_U8  },
		[IPSET_ATTR_TIMEOUT]  = { .type = NLA_U32 },
	},
	.adt_policy = {
		[IPSET_ATTR_IP]       = { .type = NLA_NESTED },
		[IPSET_ATTR_IP_TO]    = { .type = NLA_NESTED },
		[IPSET_ATTR_IP2]      = { .type = NLA_NESTED },
		[IPSET_ATTR_CIDR]     = { .type = NLA_U8 },
		[IPSET_ATTR_TIMEOUT]  = { .type = NLA_U32 },
		[IPSET_ATTR_LINENO]   = { .type = NLA_U32 },
	},
	.me = THIS_MODULE,
};

static int __init
hash_iplookup_init(void)
{
	return ip_set_type_register(&hash_iplookup_type);
}

static void __exit
hash_iplookup_fini(void)
{
	ip_set_type_unregister(&hash_iplookup_type);
}

module_init(hash_iplookup_init);
module_exit(hash_iplookup_fini);
