#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
  FILE* f = fopen(filename, "r+");
	fseek(f, 0, SEEK_END);
	uint32_t size = ftell(f);//获取文件大小size
	printf("%d\n", size);
	char buf[size];
	memset(buf, 0, sizeof(buf))
	fread(buf, size, 1, f);
	fclose(f);
	SDL_Surface *ret = STBIMG_LoadFromMemory(buf, size);
	//free(buf);
	return ret;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
