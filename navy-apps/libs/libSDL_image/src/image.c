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
	void* buf = malloc(size);//申请一段大小为size的内存空间
	fseek(f, 0, SEEK_SET);
	fread(buf, size, 1, f);//将整个文件读取到buf中
	SDL_Surface *ret = STBIMG_LoadFromMemory(buf, size);//将buf和size作为参数，调用函数，返回SDL_Surface指针
	fclose(f);//关闭文件
	free(buf);//释放内存
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
