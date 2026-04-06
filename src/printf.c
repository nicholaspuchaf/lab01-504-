#include <stdarg.h>
#include <kernel/printf.h>
#include <kernel/types.h>
#include <kernel/string.h>

// TODO: we need to implement some sort of locking/buffering mechanism
// in order to avoid interleaved prints across harts. we will purposefully
// ignore this for now

char *uart = (char *)0x10000000;
void putchar(char c)
{
	*uart = c;
	return;
}

void print(const char *str)
{
	while (*str != '\0') {
		putchar(*str);
		str++;
	}
	return;
}

static char *digits = "0123456789abcdef";
// FIXME: this needs to read signed integers as well
static char *number(char *str, size_t size, u64 number, u64 base)
{
	if (number == 0) {
		*str++ = '0';
		return str;
	}

	// this is taken from linux's implementation.
	// haven't really looked into the reasoning for
	// choosing 3 specifically
	char tmp[32] = {0};
	size_t i = 0;
	while (number > 0) {
		tmp[i++] = digits[number % base];
		number /= base;
	}

	while (i-- > 0 && --size > 0) {
		*str++ = tmp[i];
	}

	return str;
}

static char *string(char *buf, size_t size, char *str)
{
	while (size > 0 && *str != 0) {
		*buf++ = *str++;
		size--;
	}

	return buf;
}

// this is roughly inspired by kernel/printk/printk.c
// within the linux kernel

enum fmt_state {
	FMT_STATE_NONE,
	FMT_STATE_PERCENTAGE,
	FMT_STATE_NUMBER,
};

struct fmt {
	const char *str;
	enum fmt_state state;
};

static void vsnprintf(char *buf, size_t size, const char *fmt_str, va_list args)
{
	struct fmt fmt = { .str = fmt_str, .state = FMT_STATE_NONE };
	char *str = buf;
	size_t remaining = size;
	while (*fmt.str != '\0' && size > 1) {
		remaining = size - (size_t) (str - buf);
		switch (fmt.state) {
		case FMT_STATE_NONE: {
			if (*fmt.str == '%') {
				fmt.str++;
				fmt.state = FMT_STATE_PERCENTAGE;
				continue;
			} else {
				*str++ = *fmt.str++;
				continue;
			}
		}
		case FMT_STATE_PERCENTAGE: {
			if (*fmt.str == 'd') {
				fmt.str++;
				u64 n = va_arg(args, u64);
				str = number(str, remaining, n, 10);
			} else if (*fmt.str == 'x') {
				fmt.str++;
				u64 n = va_arg(args, u64);
				str = number(str, remaining, n, 16);
			} else if (*fmt.str == 's') {
				fmt.str++;
				char *s = va_arg(args, char *);
				str = string(str, remaining, s);
			}
			fmt.state = FMT_STATE_NONE;
		}
		default: {
		}
		}
	}

	*str = '\0';
}

#define PRINTK_BUF_SIZE 4096

static int current_level = LOG_INFO;

void printk_set_level(int level)
{
	current_level = level;
}

static char g_buf[PRINTK_BUF_SIZE];
void printk(int level, const char *fmt_str, ...)
{
	if (level > current_level)
		return;

	va_list args;
	va_start(args, fmt_str);
	vsnprintf(g_buf, PRINTK_BUF_SIZE, fmt_str, args);
	print(g_buf);
}

void vprintk(int level, const char *fmt_str, va_list args)
{
	if (level > current_level)
		return;

	vsnprintf(g_buf, PRINTK_BUF_SIZE, fmt_str, args);
	print(g_buf);
}
