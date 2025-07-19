#include "log.h"
#include <stdlib.h>
#include "daemon.h"

log_t* log_create() {
	log_t* self = calloc(1, sizeof(log_t));
	pthread_mutex_init(&self->mutex, NULL);
	self->level = LL_ALL;
	self->type = LT_STD;
	self->out = stdout;

	return self;
}

void logging(LOG_LEVEL level, const char* module, const char* fmt, ...) {
	log_t* self = DAEMON.LOG;
	if(level < self->level)
		return;

	va_list args;
	va_start(args, fmt);

	pthread_mutex_lock(&self->mutex);
	fprintf(self->out, "[%s] ", module);
	vfprintf(self->out, fmt, args);
	fprintf(self->out, "\n");
	pthread_mutex_unlock(&self->mutex);

	va_end(args);
}
