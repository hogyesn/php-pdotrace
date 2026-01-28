#include "logger.h"

static const logger_methods *active_logger = NULL;

int set_logger(const logger_methods *methods)
{
	active_logger = methods;
	return 0;
}

int logger_init(void)
{
	if (active_logger && active_logger->init) {
		return active_logger->init();
	}

	return 0;
}

void logger_write(const char *buf, size_t len)
{
	if (active_logger && active_logger->write) {
		active_logger->write(buf, len);
	}
}

void logger_flush(void)
{
	if (active_logger && active_logger->flush) {
		active_logger->flush();
	}
}

void logger_shutdown(void)
{
	if (active_logger && active_logger->shutdown) {
		active_logger->shutdown();
	}
}
