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
static char key_buf[64], *key_action, *key_key;
//To Be Fast
static int inline read_keyinfo(uint8_t *type, uint8_t *sym){
  int ret = NDL_PollEvent(key_buf, sizeof(key_buf));
  if (!ret){
    return 0;
  }
  //printf("%s\n", key_buf);
  key_action = key_buf;
  int i;
  for (i = 0; key_buf[i] != ' '; i++){}
  key_buf[i] = '\0';
  key_key = &key_buf[i + 1]; 
  
  //截断\n
  for (i = 0;  key_key[i] != '\0' && key_key[i] != '\n'; i++){}
  if (key_key[i] == '\n'){
    key_key[i] = '\0';
  }
  
  //strcmp("kd", key_action) == 0
  if (key_action[1] == 'd'){//加速！！
    *type = SDL_KEYDOWN;
  }else{
    *type = SDL_KEYUP;
  }

  for (i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    //剪枝掉很多
    if (key_key[0] == keyname[i][0] && strcmp(key_key, keyname[i]) == 0){
      *sym = i;
      //printf("%d %d\n", *type, *sym);
      return ret;
    }
  }
}
int SDL_PollEvent(SDL_Event *ev) {
  uint8_t type = 0, sym = 0;

  if (read_keyinfo(&type, &sym)){
    ev->type = type;
    ev->key.keysym.sym = sym;

    switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      //printf("%d Down\n", (int)sym);
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      //printf("%d Up\n", (int)sym);
      break;
    }
  }else {
    return 0;
  }

  return 1;
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	
	//if(!NDL_PollEvent(buf, sizeof(buf)))
		//return 0;
	
	//if(buf[1] == 'd')
		ev->type = SDL_KEYDOWN;
	//else if(buf[1] == 'u')
		//ev->type = SDL_KEYUP;
	//else
		//printf("%s\n", buf);
	
	//for(int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++)
	//{
		//char revise_char[64];
		//memset(revise_char, 0, sizeof(revise_char));
		//strcat(revise_char, keyname[i]);
		//strcat(revise_char, "\n");
		//if(strcmp(&buf[3], revise_char) == 0)
		//{
			//ev->key.keysym.sym = i;
			//if(ev->type == SDL_KEYDOWN)
				//key_state[i] = 1;
			//else
				//key_state[i] = 0;
			//break;
		//}
	//}
	//return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
    uint8_t type = 0, sym = 0;

  if (read_keyinfo(&type, &sym)){
    event->type = type;
    event->key.keysym.sym = sym;

    switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      //printf("%d Down\n", (int)sym);
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      //printf("%d Up\n", (int)sym);
      break;
    }
  }else {
    return 0;
  }

  return 1;
	//char buf[64];
	//memset(buf, 0, sizeof(buf));
	
	//while(!NDL_PollEvent(buf, sizeof(buf))){}
	
	//if(buf[1] == 'd')
		//event->type = SDL_KEYDOWN;
	//else if(buf[1] == 'u')
		//event->type = SDL_KEYUP;
	//else
	//	printf("%s\n", buf);
	
	//for(int i = 0; i < sizeof(keyname)/sizeof(keyname[0]); i++)
	//{
		//char revise_char[64];
		//memset(revise_char, 0, sizeof(revise_char));
		//strcat(revise_char, keyname[i]);
		//strcat(revise_char, "\n");
		//if(strcmp(&buf[3], revise_char) == 0)
		//{
			//if(event->type == SDL_KEYDOWN)
			//	key_state[i] = 1;
			//else
			//	key_state[i] = 0;
			//event->key.keysym.sym = i;
			//break;
		//}
//	}
	//return 1;
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
