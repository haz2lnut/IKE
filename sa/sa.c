#include "sa.h"
#include <libconfig.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

static const char* MM = "SA";

uint8_t sa_create_by_conf(sa_t** sa_entry, const char* filename) {
	config_t cnf;
	uint8_t len = 0;
	const char* buf;

	config_init(&cnf);

	if(!config_read_file(&cnf, filename)) {
		logging(LL_ERR, MM, "Failed to read config file(%s)", filename);
		config_destroy(&cnf);
		return len;
	}

	config_setting_t* peers = config_lookup(&cnf, "peers");
	if(peers != NULL) {
		len = config_setting_length(peers);

		for(int i = 0; i < len; i++) {
			sa_entry[i] = calloc(1, sizeof(sa_t));
			logging(LL_INFO, MM, "[peer %d]", i+1);
			config_setting_t* cur = config_setting_get_elem(peers, i);

			config_setting_t* local = config_setting_lookup(cur, "local");
			if(local != NULL) {
				logging(LL_INFO, MM, "  local");
				if(config_setting_lookup_string(local, "ip", &buf)) {
					logging(LL_INFO, MM, "    ip: %s", buf);
					sa_entry[i]->local.addr = ip4_stoa(buf);
				}
			}

			config_setting_t* remote = config_setting_lookup(cur, "remote");
			if(remote != NULL) {
				logging(LL_INFO, MM, "  remote");
				if(config_setting_lookup_string(remote, "ip", &buf)) {
					sa_entry[i]->remote.addr = ip4_stoa(buf);
					logging(LL_INFO, MM, "    ip: %s", buf);
				}
			}

			if(config_setting_lookup_string(cur, "secret", &buf)) {
				sa_entry[i]->secret = calloc(strlen(buf), sizeof(char));
				memcpy(sa_entry[i]->secret, buf, strlen(buf));
				logging(LL_INFO, MM, "  secret: %s", buf);
			}

		}
	}
	
	return len;
}
