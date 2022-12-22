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
	char buf[64];
	int if_event;
	if_event = NDL_PollEvent(buf, sizeof(buf));
	if(buf[1] == 'd')
		ev->type = SDL_KEYDOWN;
	else if(buf[1] == 'u')
		ev->type = SDL_KEYUP;
	else
		printf("Not implemented yet\n");
	for(int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++)
	{
		char revise_char[64];
		memset(revise_char, 0, sizeof(revise_char));
		strcat(revise_char, keyname[i]);
		strcat(revise_char, "\n");
		if(strcmp(&buf[3], revise_char) == 0)
		{
			ev->key.keysym.sym = i;
			break;
		}
	}
	if(if_event)
		return 1;
	else
		return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
	while(!NDL_PollEvent(buf, sizeof(buf))){}
	if(buf[1] == 'd')
		event->type = SDL_KEYDOWN;
	else if(buf[1] == 'u')
		event->type = SDL_KEYUP;
	else
		printf("Not implemented yet\n");
	for(int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++)
	{
		char revise_char[64];
		memset(revise_char, 0, sizeof(revise_char));
		strcat(revise_char, keyname[i]);
		strcat(revise_char, "\n");
		if(strcmp(&buf[3], revise_char) == 0)
		{
			event->key.keysym.sym = i;
			break;
		}
	}
	return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
