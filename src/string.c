#include <kernel/string.h>

void memset(void *dest, int val, size_t count)
{
	uint8_t *ptr = (uint8_t *) dest;
	while (count--) {
		*ptr++ = (uint8_t) val;
	}
}

void memset64(u64 *dest, u64 val, size_t count)
{
	while (count--) {
		*dest++ = val;
	}
}

char *strncpy(char *dst, const char *src, size_t n)
{
	while (n > 1 && *src != '\0') {
		*dst++ = *src++;
		n--;
	}

	dst[n] = 0;

	return dst;
}

int strcmp(const char *str1, const char *str2)
{
	while (*str1 != '\0') {
		if (*str1++ != *str2++)
			break;
	}

	return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int strncmp(const char *str1, const char *str2, size_t n)
{
	while (*str1 != '\0') {
		if (n-- == 0)
			return 0;
		if (*str1++ != *str2++)
			break;
	}

	return *(unsigned char*)str1 - *(unsigned char*)str2;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (*str++ != '\0') len++;
	return len;
}

char *strstr(char *str, const char *substring)
{
	size_t i, j;
	bool match;

	for (i = 0; str[i] != '\0'; i++) {
		match = true;
		for (j = 0; substring[j] != '\0' && str[i + j] != '\0'; j++) {
			if (str[i + j] != substring[j]) {
				match = false;
				break;
			}
		}

		if (match)
			return &str[i];
	}

	return NULL;
}
