#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t key_state[sizeof(keyname)/sizeof(keyname[0])] = {0};

int SDL_PushEvent(SDL_Event *ev) {
	assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {

	char buf[64];
	memset(buf, 0, sizeof(buf));
	
	if(!NDL_PollEvent(buf, sizeof(buf)))
		return 0;
	
	if(buf[1] == 'd')
		ev->type = SDL_KEYDOWN;
	else if(buf[1] == 'u')
		ev->type = SDL_KEYUP;
	else
		printf("%s\n", buf);
	
	for(int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++)
	{
		char revise_char[64];
		memset(revise_char, 0, sizeof(revise_char));
		strcat(revise_char, keyname[i]);
		strcat(revise_char, "\n");
		if(strcmp(&buf[3], revise_char) == 0)
		{
			ev->key.keysym.sym = i;
			if(ev->type == SDL_KEYDOWN)
				key_state[i] = 1;
			else
				key_state[i] = 0;
			break;
		}
	}
	return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  
	char buf[64];
	memset(buf, 0, sizeof(buf));
	
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
			if(event->type == SDL_KEYDOWN)
				key_state[i] = 1;
			else
				key_state[i] = 0;
			event->key.keysym.sym = i;
			break;
		}
	}
	return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
	assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  if(numkeys != NULL);
		*numkeys = sizeof(keyname)/sizeof(keyname[0]);
	return key_state;
}
