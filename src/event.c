/* Icecast
 *
 * This program is distributed under the GNU General Public License, version 2.
 * A copy of this license is included with this source.
 *
 * Copyright 2000-2004, Jack Moffitt <jack@xiph.org, 
 *                      Michael Smith <msmith@xiph.org>,
 *                      oddsock <oddsock@xiph.org>,
 *                      Karl Heyes <karl@xiph.org>
 *                      and others (see AUTHORS for details).
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "event.h"
#include "cfgfile.h"
#include "yp.h"
#include "source.h"

#include "refbuf.h"
#include "client.h"
#include "logging.h"
#include "slave.h"

#define CATMODULE "event"

void event_config_read(void *arg)
{
    int ret;
    ice_config_t *config;
    ice_config_t new_config;
    /* reread config file */

    config = config_grab_config(); /* Both to get the lock, and to be able
                                     to find out the config filename */
    ret = config_parse_file(config->config_filename, &new_config);
    if(ret < 0) {
        ERROR0("Error parsing config, not replacing existing config");
        switch(ret) {
            case CONFIG_EINSANE:
                ERROR0("Config filename null or blank");
                break;
            case CONFIG_ENOROOT:
                ERROR1("Root element not found in %s", config->config_filename);
                break;
            case CONFIG_EBADROOT:
                ERROR1("Not an icecast2 config file: %s",
                        config->config_filename);
                break;
            default:
                ERROR1("Parse error in reading %s", config->config_filename);
                break;
        }
        config_release_config();
    }
    else {
        config_clear(config);
        config_set_config(&new_config);
        restart_logging (config_get_config_unlocked());
        slave_recheck();
        yp_recheck_config (config_get_config_unlocked());
        source_update (config_get_config_unlocked());

        config_release_config();
    }
}

