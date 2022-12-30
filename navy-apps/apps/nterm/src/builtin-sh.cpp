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
	memset(command_record, 0, sizeof(command_record));
	int i = 0;
	while(cmd[i] != '\0')
	{
		command_record[i] = cmd[i];
		i++;
	}
	command_record[i-1] = '\0'; //处理换行符
	char *argv[16];
	printf("%c\n", argv[0][0]);
	
	i = 0;
	int j = 0;
	while(command_record[i] != '\0')
	{
		printf("i = %d j = %d\n", i, j);
		int one_command_count = 0;
		while(command_record[i] != ' ')
		{
			argv[j][one_command_count] = command_record[i];
			i++;
			one_command_count++;
		}
		argv[j][one_command_count] = '\0';
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
