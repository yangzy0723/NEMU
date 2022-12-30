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
	char command_record[4096];
	int i = 0;
	while(cmd[i] != '\0')
	{
		command_record[i] = cmd[i];
		i++;
	}
	command_record[i-1] = '\0'; //处理换行符
	printf("%s\n", command_record);
	char *argv[16];
	
	i = 0;
	int j = 0;
	while(command_record[i] != '\0')
	{
		int one_command_count = 0;
		while(command_record[i] != ' ')
		{
			argv[j][one_command_record] = command[i];
			i++;
			one_command_record++;
		}
		argv[j][one_command_record] = '\0';
		if(command_record[i] == '\0')//说明最后一个命令已经传进去了
			break;
		i++;
		j++;
	}
	argv[j+1] = NULL;//要求是argv最后一个必须是NULL
	
	execvp(argv[0], argv);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();
	print("123\n");
	setenv("PATH", "/bin", 0);

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
