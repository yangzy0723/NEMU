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
	memset((char *)buf, 0, strlen((char *)buf));//每用完一次，将整个以buf指针为首的字符串清0
	if(ev.keydown)
		strcat((char*)buf, "kd ");
	else
		strcat((char*)buf, "ku ");
	strcat((char*)buf, keyname[ev.keycode]);
	*(char *)(buf + strlen((char *)buf) + 1) = '\n';
	*(char *)(buf + strlen((char *)buf) + 1) = 0;
	printf("%s\n", buf);
	printf("%d\n", strlen(buf));
	return strlen((char *)buf);
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
