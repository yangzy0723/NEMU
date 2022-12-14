#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(int i = 0; i < len; i++)
		putch(*(((char *)buf) + i));
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	if(ev.keycode == AM_KEY_NONE)
		return 0;
	char* keyboard_code;
	printf("%d\n", ev.keydown);
	if(ev.keydown)
		keyboard_code = "kd ";
	else
		keyboard_code = "ku ";
	strcat(keyboard_code, keyname[ev.keycode]);

	//assert(len >= strlen(keyboard_code) + 1);
	printf("buf = %p, keyboard_code = %s, %d\n", buf, keyboard_code, strlen(keyboard_code));	
	memcpy(buf, (const void *)keyboard_code, strlen(keyboard_code));	
	*(char *)(buf + strlen(keyboard_code) + 1) = '\n';
	return strlen(keyboard_code) + 1;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
