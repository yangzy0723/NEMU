#include <NDL.h>
#include <SDL.h>

#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
        "NONE",
        _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
    return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
    int Size = sizeof(keyname);
    char buf[64];
    if (NDL_PollEvent(buf, sizeof(buf)) != 0) {
        //assert(buf[0] == 'k');
        ev->type = (buf[1] == 'u') ? SDL_KEYUP : SDL_KEYDOWN;
        if (ev->type == SDL_KEYDOWN) {
            for (int i = 0; Size; ++i, Size -= sizeof(keyname[i]))
                if (strcmp(&buf[3], keyname[i]) == 0) {
                    ev->key.keysym.sym = i;
                    return 1;
                }
        }
    }
    return 1;
    return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
    int Size = sizeof(keyname);
    char buf[64];
    while (1)
        if (NDL_PollEvent(buf, sizeof(buf)) != 0) {
            //assert(buf[0] == 'k');
            event->type = (buf[1] == 'u') ? SDL_KEYUP : SDL_KEYDOWN;
            if (event->type == SDL_KEYDOWN) {
                for (int i = 0; Size; ++i, Size -= sizeof(keyname[i]))
                    if (strcmp(&buf[3], keyname[i]) == 0) {
                        event->key.keysym.sym = i;
                        return 1;
                    }
            }
        }
    return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
    return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys) {
    return NULL;
}
