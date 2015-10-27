#include <sys/stat.h>
#include <unistd.h>

#include "BSP/devboard/uart.h"

#include <stm32f4xx.h>
#define STACK_POINTER (__get_MSP())

// #define UNUSED(x) ((void)x)

int _close(int file) {
	UNUSED(file);
	return -1;
}

int _fstat(int file, struct stat *st) {
	UNUSED(file);
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file) {
	UNUSED(file);
	return 1;
}

int _lseek(int file, int ptr, int dir) {
	UNUSED(file); UNUSED(ptr); UNUSED(dir);
	return 0;
}

int _open(const char *name, int flags, int mode) {
	UNUSED(name); UNUSED(flags); UNUSED(mode);
	return -1;
}

int _read(int file, char *ptr, int len) {
	UNUSED(file);

	if(len == 0) return 0;

	int todo;
	for(todo = 0; todo < len; todo++) {
		UNUSED(*ptr++);
	}
	return todo;
}

caddr_t _sbrk(int incr) {
	extern char _end;		/* Defined by the linker */
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &_end;
	}
	prev_heap_end = heap_end;
	if ((unsigned)(heap_end + incr) > STACK_POINTER) {
		/* Heap and stack collision */
		return (caddr_t)0;
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len) {
	// UNUSED(file);

	for (int todo = 0; todo < len; todo++) {
		const char c = *ptr++;
		if (isatty(file) && c == '\n') BSP_UARTx_transmit((uint8_t*)&(char){'\r'}, 1);
		if (BSP_UARTx_transmit((uint8_t*)&c, 1) != 0) break;
	}

	return len;
	// return BSP_UARTx_transmit((uint8_t*)ptr, len) == 0 ? len : 0;
}

void _exit (int exitcode) {
	(void)exitcode;
	while(1);
}

void _init(void) {}
