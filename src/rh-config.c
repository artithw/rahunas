/**
 * RahuNAS configuration
 * Author: Suriya Soutmun <darksolar@gmail.com>
 * Date:   2008-11-26
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <syslog.h>
#include <dirent.h>
#include <errno.h>

#include "rahunasd.h"
#include "rh-config.h"

GList *interfaces_list = NULL;
static unsigned long ifb_reserved = 0;

enum lcfg_status rahunas_visitor(const char *key, void *data, size_t size, 
                                 void *user_data) {
  char *value = strndup(data, size);
  union rahunas_config *config = (union rahunas_config *)user_data;
  char *clone_key = NULL;
  char *sep = NULL; 
  char *main_key = NULL;
  char *sub_key = NULL;
  enum config_type cfg_type;
  int  valid = 1;

  if(config == NULL)
    return lcfg_status_error;

  if(value == NULL)
    return lcfg_status_error;

  clone_key = strdup(key);
  if (clone_key == NULL)
    return lcfg_status_error;

  sep = strstr(clone_key, ".");
  main_key = clone_key;
  sub_key = sep + 1;
  *sep = '\0';

  if (strncmp(main_key, "main", 4) == 0) {
    cfg_type = MAIN;
  } else if (strncmp(main_key, "service_class", strlen ("service_class")) == 0) {
    cfg_type = SERVICECLASS;
  } else {
    cfg_type = VSERVER;
    if (config->rh_vserver.vserver_name == NULL)
      config->rh_vserver.vserver_name = strdup(main_key);
  }

  switch (cfg_type) {
    case MAIN:
      if (strncmp(sub_key, "conf_dir", 8) == 0) {
        if (config->rh_main.conf_dir != NULL)
          free(config->rh_main.conf_dir);
        config->rh_main.conf_dir = strdup(value);
      } else if (strncmp(sub_key, "serviceclass_conf_dir", 21) == 0) {
        if (config->rh_main.serviceclass_conf_dir != NULL)
          free(config->rh_main.serviceclass_conf_dir);
        config->rh_main.serviceclass_conf_dir = strdup(value);
      } else if (strncmp(sub_key, "log_file", 8) == 0) {
        if (config->rh_main.log_file != NULL)
          free(config->rh_main.log_file);
        config->rh_main.log_file = strdup(value); 
      } else if (strncmp(sub_key, "dhcp", 4) == 0) {
        if (config->rh_main.dhcp != NULL)
          free(config->rh_main.dhcp);
        config->rh_main.dhcp = strdup(value);
      } else if (strncmp(sub_key, "serviceclass_attribute", strlen ("serviceclass_attribute")) == 0) {
        // Do nothing, no needs to collect this value
      } else if (strncmp(sub_key, "serviceclass", 12) == 0) {
        config->rh_main.serviceclass = strncmp(value, "yes", 3) == 0 ? 1 : 0;
      } else if (strncmp(sub_key, "bandwidth_shape", 15) == 0) {
        config->rh_main.bandwidth_shape = strncmp(value, "yes", 3) == 0 ? 1 : 0;
      } else if (strncmp(sub_key, "bittorrent_download_max", 23) == 0) {
        config->rh_main.bittorrent_download_max = atoi(value); 
      } else if (strncmp(sub_key, "bittorrent_upload_max", 21) == 0) {
        config->rh_main.bittorrent_upload_max = atoi(value); 
      } else if (strncmp(sub_key, "polling_interval", 16) == 0) {
        config->rh_main.polling_interval = atoi(value);
      }      
      break;

    case SERVICECLASS:
      if (strncmp (sub_key, "serviceclass_id",
            strlen("serviceclass_id")) == 0) {
        config->rh_serviceclass.serviceclass_id = atoi (value);
      } else if (strncmp (sub_key, "serviceclass_name", strlen("serviceclass_name")) == 0) {
        if (config->rh_serviceclass.serviceclass_name != NULL)
          free(config->rh_serviceclass.serviceclass_name);
        config->rh_serviceclass.serviceclass_name = strdup(value);
      } else if (strncmp (sub_key, "description", strlen("description")) == 0) {
        if (config->rh_serviceclass.description != NULL)
          free(config->rh_serviceclass.description);
        config->rh_serviceclass.description = strdup(value);
      } else if (strncmp (sub_key, "network", strlen("network")) == 0) {
        if ((sep = strchr (value, '/')) != NULL) {
          if (config->rh_serviceclass.network != NULL)
            free(config->rh_serviceclass.network);

          config->rh_serviceclass.network = strdup(value);

          sep++;
          config->rh_serviceclass.network_size = atoi (sep);

          if (config->rh_serviceclass.network_size != 0) {
            config->rh_serviceclass.network_size =
              (1 << (32 - config->rh_serviceclass.network_size)) - 2;
          }

          sep = strsep (&value, "/");
          if (sep != NULL) {
            if (inet_aton (sep, &config->rh_serviceclass.start_addr) == 0)
              {
                valid = 0;
              }
            else
              {
                DP ("service_class: %s - start ip = %s, size: %d",
                    config->rh_serviceclass.serviceclass_name,
                    inet_ntoa (config->rh_serviceclass.start_addr),
                    config->rh_serviceclass.network_size);
              }
          } else {
            config->rh_serviceclass.network_size = 0;
            valid = 0;
          }
        } else {
          valid = 0;
        }

        if (!valid) {
          if (config->rh_serviceclass.serviceclass_name != NULL) {
            syslog(LOG_ERR, "\"%s\" service_class config config error: "
                            "invalid network %s",
                            config->rh_serviceclass.serviceclass_name, value);
          } else {
            syslog(LOG_ERR, "unknown service_class config error: "
                            "invalid network %s", value);
          }
        }
      } else if (strncmp (sub_key, "fake_arpd_iface",
                 strlen("fake_arpd_iface")) == 0) {
        if (config->rh_serviceclass.fake_arpd_iface != NULL)
          free(config->rh_serviceclass.fake_arpd_iface);
        config->rh_serviceclass.fake_arpd_iface = strdup(value);
      } else if (strncmp (sub_key, "fake_arpd", strlen("fake_arpd")) == 0) {
        if (config->rh_serviceclass.fake_arpd != NULL)
          free(config->rh_serviceclass.fake_arpd);
        config->rh_serviceclass.fake_arpd = strdup(value);
      }
      break;

    case VSERVER:
      if (strncmp(sub_key, "vserver_id", 10) == 0) {
        config->rh_vserver.vserver_id = atoi(value);
      } else if (strncmp(sub_key, "dev_external", 12) == 0) {
        if (config->rh_vserver.dev_external != NULL)
          free(config->rh_vserver.dev_external);
        config->rh_vserver.dev_external = strdup(value);
      } else if (strncmp(sub_key, "dev_internal", 12) == 0) {
        if (config->rh_vserver.dev_internal != NULL)
          free(config->rh_vserver.dev_internal);
        config->rh_vserver.dev_internal = strdup(value);
      } else if (strncmp(sub_key, "vlan", 4) == 0) {
        if (config->rh_vserver.vlan != NULL)
          free(config->rh_vserver.vlan);
        config->rh_vserver.vlan = strdup(value);
      } else if (strncmp(sub_key, "vlan_raw_dev_external", 21) == 0) {
        if (config->rh_vserver.vlan_raw_dev_external != NULL)
          free(config->rh_vserver.vlan_raw_dev_external);
        config->rh_vserver.vlan_raw_dev_external = strdup(value);
       } else if (strncmp(sub_key, "vlan_raw_dev_internal", 21) == 0) {
        if (config->rh_vserver.vlan_raw_dev_internal != NULL)
          free(config->rh_vserver.vlan_raw_dev_internal);
        config->rh_vserver.vlan_raw_dev_internal = strdup(value);
      } else if (strncmp(sub_key, "bridge", 6) == 0) {
        if (config->rh_vserver.bridge != NULL)
          free(config->rh_vserver.bridge);
        config->rh_vserver.bridge = strdup(value);
      } else if (strncmp(sub_key, "masquerade", 10) == 0) {
        if (config->rh_vserver.masquerade != NULL)
          free(config->rh_vserver.masquerade);
        config->rh_vserver.masquerade = strdup(value);
      } else if (strncmp(sub_key, "ignore_mac", 10) == 0) {
        if (config->rh_vserver.ignore_mac != NULL)
          free(config->rh_vserver.ignore_mac);
        config->rh_vserver.ignore_mac = strdup(value);
      } else if (strncmp(sub_key, "vserver_ip", 10) == 0) {
        if (config->rh_vserver.vserver_ip != NULL)
          free(config->rh_vserver.vserver_ip);
        config->rh_vserver.vserver_ip = strdup(value); 
      } else if (strncmp(sub_key, "vserver_fqdn", 12) == 0) {
        if (config->rh_vserver.vserver_fqdn != NULL)
          free(config->rh_vserver.vserver_fqdn);
        config->rh_vserver.vserver_fqdn = strdup(value);
      } else if (strncmp(sub_key, "vserver_ports_allow", 19) == 0) {
        if (config->rh_vserver.vserver_ports_allow != NULL)
          free(config->rh_vserver.vserver_ports_allow);
        config->rh_vserver.vserver_ports_allow = strdup(value);
      } else if (strncmp(sub_key, "vserver_ports_intercept", 23) == 0) {
        if (config->rh_vserver.vserver_ports_intercept != NULL)
          free(config->rh_vserver.vserver_ports_intercept);
        config->rh_vserver.vserver_ports_intercept = strdup(value);
      } else if (strncmp(sub_key, "clients", 7) == 0) {
        if (config->rh_vserver.clients != NULL)
          free(config->rh_vserver.clients);
        config->rh_vserver.clients = strdup(value);
      } else if (strncmp(sub_key, "excluded", 8) == 0) {
        if (config->rh_vserver.excluded != NULL)
          free(config->rh_vserver.excluded);
        config->rh_vserver.excluded = strdup(value);
      } else if (strncmp(sub_key, "idle_timeout", 12) == 0) {
        config->rh_vserver.idle_timeout = atoi(value);
      } else if (strncmp(sub_key, "dns", 3) == 0) {
        if (config->rh_vserver.dns != NULL)
          free(config->rh_vserver.dns);
        config->rh_vserver.dns = strdup(value);
      } else if (strncmp(sub_key, "ssh", 3) == 0) {
        if (config->rh_vserver.ssh != NULL)
          free(config->rh_vserver.ssh);
        config->rh_vserver.ssh = strdup(value);
      } else if (strncmp(sub_key, "proxy", 5) == 0) {
        if (strncmp(sub_key, "proxy_host", 10) == 0) {
          if (config->rh_vserver.proxy_host != NULL)
            free(config->rh_vserver.proxy_host);
          config->rh_vserver.proxy_host = strdup(value);
        } else if (strncmp(sub_key, "proxy_port", 10) == 0) {
          if (config->rh_vserver.proxy_port != NULL)
            free(config->rh_vserver.proxy_port);
          config->rh_vserver.proxy_port = strdup(value);
        } else {
          if (config->rh_vserver.proxy != NULL)
            free(config->rh_vserver.proxy);
          config->rh_vserver.proxy = strdup(value);
        }
      } else if (strncmp(sub_key, "bittorrent", 10) == 0) {
        if (strncmp(sub_key, "bittorrent_allow", 16) == 0) {
          if (config->rh_vserver.bittorrent_allow != NULL)
            free(config->rh_vserver.bittorrent_allow);
          config->rh_vserver.bittorrent_allow = strdup(value);
        } else {
          if (config->rh_vserver.bittorrent != NULL)
            free(config->rh_vserver.bittorrent);
          config->rh_vserver.bittorrent = strdup(value);
        }
      } else if (strncmp(sub_key, "radius_host", 11) == 0) {
        if (config->rh_vserver.radius_host != NULL)
          free(config->rh_vserver.radius_host);
        config->rh_vserver.radius_host = strdup(value);
      } else if (strncmp(sub_key, "radius_secret", 13) == 0) {
        if (config->rh_vserver.radius_secret != NULL)
          free(config->rh_vserver.radius_secret);
        config->rh_vserver.radius_secret = strdup(value);
      } else if (strncmp(sub_key, "radius_encrypt", 14) == 0) {
        if (config->rh_vserver.radius_encrypt != NULL)
          free(config->rh_vserver.radius_encrypt);
        config->rh_vserver.radius_encrypt = strdup(value);
      } else if (strncmp(sub_key, "radius_auth_port", 16) == 0) {
        if (config->rh_vserver.radius_auth_port != NULL)
          free(config->rh_vserver.radius_auth_port);
        config->rh_vserver.radius_auth_port = strdup(value);
      } else if (strncmp(sub_key, "radius_account_port", 19) == 0) {
        if (config->rh_vserver.radius_account_port != NULL)
          free(config->rh_vserver.radius_account_port);
        config->rh_vserver.radius_account_port = strdup(value);
      } else if (strncmp(sub_key, "nas_identifier", 14) == 0) {
        if (config->rh_vserver.nas_identifier != NULL)
          free(config->rh_vserver.nas_identifier);
        config->rh_vserver.nas_identifier = strdup(value);
      } else if (strncmp(sub_key, "nas_port", 8) == 0) {
        if (config->rh_vserver.nas_port != NULL)
          free(config->rh_vserver.nas_port);
        config->rh_vserver.nas_port = strdup(value);
      } else if (strncmp(sub_key, "nas_login_title", 15) == 0) {
        if (config->rh_vserver.nas_login_title != NULL)
          free(config->rh_vserver.nas_login_title);
        config->rh_vserver.nas_login_title = strdup(value);
      } else if (strncmp(sub_key, "nas_default_redirect", 20) == 0) {
        if (config->rh_vserver.nas_default_redirect != NULL)
          free(config->rh_vserver.nas_default_redirect);
        config->rh_vserver.nas_default_redirect = strdup(value);
      } else if (strncmp(sub_key, "nas_default_language", 20) == 0) {
        if (config->rh_vserver.nas_default_language != NULL)
          free(config->rh_vserver.nas_default_language);
        config->rh_vserver.nas_default_language = strdup(value);
      } else if (strncmp(sub_key, "nas_weblogin_template", 21) == 0) {
        if (config->rh_vserver.nas_weblogin_template != NULL)
          free(config->rh_vserver.nas_weblogin_template);
        config->rh_vserver.nas_weblogin_template = strdup(value);
      }
      break;
  }

  
  rh_free((void **) &clone_key);

  return lcfg_status_ok;
}

int get_config(const char *cfg_file, union rahunas_config *config) {
  lcfg_visitor_function visitor_func = rahunas_visitor;
  struct lcfg *c = lcfg_new(cfg_file);
  
  syslog(LOG_INFO, "Parsing config file: %s", cfg_file);

  if (lcfg_parse(c) != lcfg_status_ok) {
    syslog(LOG_ERR, "config error: %s", lcfg_error_get(c));
    lcfg_delete(c);
    return -1;
  }

  syslog(LOG_INFO, "Processing config file: %s", cfg_file);
  if (lcfg_accept(c, visitor_func, config) != lcfg_status_ok) {
    syslog(LOG_ERR, "config error: %s", lcfg_error_get(c));
    lcfg_delete(c);
    return -1;
  }

  lcfg_delete(c);

  return 0;
}


int get_value(const char *cfg_file, const char *key, void **data, size_t *len)
{
  lcfg_visitor_function visitor_func = rahunas_visitor;
  struct lcfg *c = lcfg_new(cfg_file);
  
  if (lcfg_parse(c) != lcfg_status_ok) {
    syslog(LOG_ERR, "config error: %s", lcfg_error_get(c));
    lcfg_delete(c);
    return -1;
  }

  if (lcfg_value_get(c, key, data, len) != lcfg_status_ok) {
    lcfg_delete(c);
    return -1;
  } 

  lcfg_delete(c);

  return 0;
}

int get_vservers_config(const char *conf_dir, RHMainServer *server)
{
  DIR *dp;
  struct dirent *dirp;
  void *data = NULL;
  size_t len;
  char conf_file[200];

  if ((dp = opendir(conf_dir)) == NULL)
    return errno;
  
  while ((dirp = readdir(dp)) != NULL) {
    if (strstr(dirp->d_name, ".conf") == NULL)
      continue;

    memset(conf_file, 0, sizeof(conf_file));

    strncat(conf_file, conf_dir, sizeof(conf_file));
    strncat(conf_file, "/", 1);
    strncat(conf_file, dirp->d_name, sizeof(conf_file));

    syslog(LOG_INFO, "Loading config file: %s", conf_file);
    
    register_vserver(server, conf_file);
  }
  
  closedir(dp);
  return 0;
}

int get_serviceclass_config(const char *conf_dir, struct main_server *server)
{
  DIR *dp;
  struct dirent *dirp;
  void *data = NULL;
  size_t len;
  char conf_file[200];

  if ((dp = opendir(conf_dir)) == NULL)
    return errno;

  while ((dirp = readdir(dp)) != NULL) {
    if (strstr(dirp->d_name, ".conf") == NULL)
      continue;

    memset(conf_file, 0, sizeof(conf_file));

    strncat(conf_file, conf_dir, sizeof(conf_file));
    strncat(conf_file, "/", 1);
    strncat(conf_file, dirp->d_name, sizeof(conf_file));

    syslog(LOG_INFO, "Loading service class config file: %s", conf_file);

    register_serviceclass(server, conf_file);
  }

  closedir(dp);
  return 0;
}

int cleanup_vserver_config(struct rahunas_vserver_config *config)
{
  rh_free((void **) &config->vserver_name);
  rh_free((void **) &config->dev_external);
  rh_free((void **) &config->dev_internal);
  rh_free((void **) &config->vlan);
  rh_free((void **) &config->vlan_raw_dev_external);
  rh_free((void **) &config->vlan_raw_dev_internal);
  rh_free((void **) &config->bridge);
  rh_free((void **) &config->masquerade);
  rh_free((void **) &config->ignore_mac);
  rh_free((void **) &config->vserver_ip);
  rh_free((void **) &config->vserver_fqdn);
  rh_free((void **) &config->vserver_ports_allow);
  rh_free((void **) &config->vserver_ports_intercept);
  rh_free((void **) &config->clients);
  rh_free((void **) &config->excluded);
  rh_free((void **) &config->dns);
  rh_free((void **) &config->ssh);
  rh_free((void **) &config->proxy);
  rh_free((void **) &config->proxy_host);
  rh_free((void **) &config->proxy_port);
  rh_free((void **) &config->bittorrent);
  rh_free((void **) &config->bittorrent_allow);
  rh_free((void **) &config->radius_host);
  rh_free((void **) &config->radius_secret);
  rh_free((void **) &config->radius_encrypt);
  rh_free((void **) &config->radius_auth_port);
  rh_free((void **) &config->radius_account_port);
  rh_free((void **) &config->nas_identifier);
  rh_free((void **) &config->nas_port);
  rh_free((void **) &config->nas_login_title);
  rh_free((void **) &config->nas_default_redirect);
  rh_free((void **) &config->nas_default_language);
  rh_free((void **) &config->nas_weblogin_template);

  return 0;
}

int
cleanup_serviceclass_config(struct rahunas_serviceclass_config *config)
{
  rh_free((void **) &config->serviceclass_name);
  rh_free((void **) &config->description);
  rh_free((void **) &config->network);
  rh_free((void **) &config->fake_arpd);
  rh_free((void **) &config->fake_arpd_iface);

  return 0;
}

int cleanup_mainserver_config(struct rahunas_main_config *config)
{
  rh_free((void **) &config->conf_dir);
  rh_free((void **) &config->serviceclass_conf_dir);
  rh_free((void **) &config->log_file);
  rh_free((void **) &config->dhcp);

  return 0;
}


GList *append_interface (GList *inf, 
                         const char *inf_name)
{
  GList *runner = NULL;
  struct interfaces *iface = NULL;
  struct interfaces *item  = NULL;
  int    ifb_ifno;

  if (!inf_name)
    return inf;

  item = (struct interfaces *) malloc (sizeof (struct interfaces));
  if (!item)
    return inf;

  runner = g_list_first (inf);
  while (runner != NULL)
    {
      iface = (struct interfaces *)runner->data;
      if (iface->dev_internal &&
          strncmp(iface->dev_internal, inf_name, strlen(inf_name)) == 0)
        {
          // Already in the list
          (iface->hit)++;
          free(item); 
          return inf;
        }
      runner = g_list_next (runner);
    }

done:
  ifb_ifno = ifb_interface_reserve ();
  if (ifb_ifno < 0)
    {
      free (item);
      return inf;
    }

  strncpy(item->dev_internal, inf_name, sizeof (item->dev_internal));
  snprintf(item->dev_ifb, sizeof (item->dev_ifb), "ifb%d", ifb_ifno);
  item->init = 0;
  item->hit  = 1;
  DP ("Interface append: %s, %s", item->dev_internal, item->dev_ifb);
  
  return g_list_append (inf, item);
}

GList *remove_interface (GList *inf,
                         const char *inf_name)
{
  GList *runner = NULL;
  struct interfaces *iface = NULL;
  int    ifb_ifno;

  if (!inf_name || !inf)
    return inf;

  runner = g_list_first (inf);
  while (runner != NULL)
    {
      iface = (struct interfaces *)runner->data;
      if (iface->dev_internal &&
          strncmp (iface->dev_internal, inf_name, strlen (inf_name)) == 0)
        {
          iface->hit--;
          if (iface->hit <=0 )
            {
              sscanf (iface->dev_ifb, "ifb%d", &ifb_ifno);
              ifb_interface_release (ifb_ifno);
              if (iface)
                free (iface);

              return g_list_delete_link (inf, runner);
            }
        }

      runner = g_list_next (runner);
    }

  return inf;
}


struct interfaces *get_interface (GList *inf,
                                  const char *inf_name)
{
  GList *runner = NULL;
  struct interfaces *iface = NULL;
  
  if (!inf_name || !inf)
    return NULL;

  runner = g_list_first (inf);

  while (runner != NULL)
    {
      iface = (struct interfaces *) runner->data;
      if (strncmp (iface->dev_internal, inf_name, strlen (inf_name)) == 0)
        return iface;

      runner = g_list_next (runner);
    } 

  return NULL;
}

int ifb_interface_reserve (void)
{
  int i;
  unsigned long mask = 1;

  for (i=0; i < MAX_IFB_IFACE; i++)
    {
      mask = 1 << i;
      if (!(ifb_reserved & mask))
        {
          ifb_reserved |= mask;
          return i;
        } 
    }

  return -1;
}

void ifb_interface_release (int ifno)
{
  unsigned long mask = 1;

  mask <<= ifno;
  mask = ~mask;
  ifb_reserved &= mask;
}
