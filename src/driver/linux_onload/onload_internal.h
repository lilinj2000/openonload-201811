/*
** Copyright 2005-2018  Solarflare Communications Inc.
**                      7505 Irvine Center Drive, Irvine, CA 92618, USA
** Copyright 2002-2005  Level 5 Networks Inc.
**
** This program is free software; you can redistribute it and/or modify it
** under the terms of version 2 of the GNU General Public License as
** published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
*/

#ifndef __ONLOAD_INTERNAL_H__
#define __ONLOAD_INTERNAL_H__

#include <linux/init.h>


extern int oo_driverlink_register(void);
extern void oo_driverlink_unregister(void);

#ifdef EFRM_HAVE_NF_NET_HOOK
struct net;
int oo_register_nfhook(struct net *net);
void oo_unregister_nfhook(struct net *net);
#endif

extern int  ci_install_proc_entries(void);
extern void ci_uninstall_proc_entries(void);

struct proc_dir_entry;
extern struct proc_dir_entry* oo_proc_root;

extern int __init oo_epoll_chrdev_ctor(void);
extern void oo_epoll_chrdev_dtor(void);

extern int __init onloadfs_init(void);
extern void onloadfs_fini(void);

#include <onload/fd_private.h>
void onload_priv_free(ci_private_t *priv);

extern int cp_server_pids_open(struct inode *inode, struct file *file);
extern int cp_proc_stats_open(struct inode *inode, struct file *file);

/* Temporarily empower the current process with CAP_NET_RAW. */
static inline const struct cred *
oo_cplane_empower_cap_net_raw(struct net* netns)
{
#ifdef EFRM_NET_HAS_USER_NS
  if( ! ns_capable(netns->user_ns, CAP_NET_RAW) ) {
#else
  if( ! capable(CAP_NET_RAW) ) {
#endif
    struct cred *creds = prepare_creds();
    if( creds != NULL ) {
      creds->cap_effective.cap[0] |= 1 << CAP_NET_RAW;
      return override_creds(creds);
    }
  }
  return NULL;
}
static inline void
oo_cplane_drop_cap_net_raw(const struct cred *orig_creds)
{
  if( orig_creds != NULL )
    revert_creds(orig_creds);
}

#endif  /* __ONLOAD_INTERNAL_H__ */