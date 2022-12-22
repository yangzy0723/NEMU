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
	uint32_t png_size = ftell(f);//获取文件大小size
	char buf[png_size];
	printf("1\n");
	memset(buf, 0, png_size);
	printf("2\n");
	fread(buf, png_size, 1, f);
	printf("3\n");
	fclose(f);
	SDL_Surface *ret = STBIMG_LoadFromMemory(buf, png_size);
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
