#include "daemon.h"

int main() {
	daemon_create();
	DAEMON.sa_len = sa_create_by_conf(DAEMON.SAS, "conf/local.conf");
	running();
	
	return 0;
}
