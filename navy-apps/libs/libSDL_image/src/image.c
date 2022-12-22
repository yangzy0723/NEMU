#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION

#include <stdio.h>
#include "SDL_stbimage.h"

SDL_Surface *IMG_Load_RW(SDL_RWops *src, int freesrc) {
    assert(src->type == RW_TYPE_MEM);
    assert(freesrc == 0);
    return NULL;
}

SDL_Surface *IMG_Load(const char *filename) {
    FILE *fp = fopen(filename, "r");
    assert(fp);
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    char *buf = (char *) malloc(size);
    assert(buf);
    fseek(fp, 0, SEEK_SET);
    for (; (*buf = fgetc(fp)) != EOF; ++buf);
    SDL_Surface *ret = STBIMG_LoadFromMemory(buf, size);
    fclose(fp);
    free(buf);
    return ret;
}

int IMG_isPNG(SDL_RWops *src) {
    return 0;
}

SDL_Surface *IMG_LoadJPG_RW(SDL_RWops *src) {
    return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
    return "Navy does not support IMG_GetError()";
}
