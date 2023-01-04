#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
	char command_record[256];
	memset(command_record, 0, sizeof(command_record));

	int i = 0;
	while(cmd[i] != '\0')
	{
		command_record[i] = cmd[i];
		i++;
	}
	char *command_split[16];
	i = 0;
	int memory_i = 0;
	int j = 0;
	while(command_record[i] != '\0')
	{
		memory_i = i;//记录原有的i
		while(command_record[i] != ' ' && command_record[i] != '\n')
		{
			i++;
		}
		command_record[i] = '\0';
		command_split[j] = &command_record[memory_i]; 
		i++;
		j++;
	}
  command_split[j] = NULL;//要求是argv最后一个必须是NULL
	
	char app[16] = "/bin/";//这个不能开太小，不然装不下完整的应用程序
	strcat(app, command_split[0]);//第一个是app的名字
	char **argv = &command_split[1];
	char *envp[] = {NULL};
	
	printf("go to file: %s\n", app);
	execve(app, argv, envp);//学着menu的实现来
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
