#include "logger.h"
#include <stdio.h>
#include "php.h"
#include "php_pdotrace.h"

static FILE *fp = NULL;

static int file_logger_init(void)
{
	const char *path = PDOTRACE_G(log_file_path);

	if (!path || !*path) {
		return 0; // logging disabled
	}

	fp = fopen(path, "a");
	return fp ? 0 : -1;
}

static void file_logger_write(const char *buf, size_t len)
{
	if (fp) {
		fwrite(buf, 1, len, fp);
		// fwrite("\n", 1, 1, fp);
	}
}

static void file_logger_flush(void)
{
	if (fp) {
		fflush(fp);
	}
}

static void file_logger_shutdown(void)
{
	if (fp) {
		fflush(fp);
		fclose(fp);
		fp = NULL;
	}
}

static const logger_methods file_logger_methods = {
	.name     = "file",
	.init     = file_logger_init,
	.write    = file_logger_write,
	.flush    = file_logger_flush,
	.shutdown = file_logger_shutdown
};

const logger_methods *file_logger_get_methods(void)
{
	return &file_logger_methods;
}
