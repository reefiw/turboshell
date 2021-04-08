#include "minishell.h"

static	int	ctrl_d(t_tsh *tsh)
{
	free(tsh->line);
	tsh->end_line = 1;
	tsh->line = ft_strdup("exit");
	if (!tsh->line)
		return(error_handler("memmory doesn't allocated"));
	return(0);
}

static	int	init_shell(t_tsh *tsh)
{
	char *term_type;

	term_type = getenv("TERM");
	tcgetattr(0, &tsh->term);
	tsh->term.c_lflag &= ~ECHO;
	tsh->term.c_lflag &= ~ICANON;
	tsh->term.c_lflag &= ~ISIG;
	tsh->term.c_cc[VMIN] = 1;
	tsh->term.c_cc[VTIME] = 1;
	tcsetattr(0, TCSANOW, &tsh->term);
	if (!term_type || tgetent(0, TERM_NAME) != 1) // term_type -> TERM  term_type -> !term_type
		return (error_handler("Can not find terminal or termcap base."));
	tsh->hfd = open("tsh_history", O_CREAT | O_WRONLY | O_APPEND, 0755);
	tsh->is_running = 1;
	tsh->symbols = 0;
	tsh->end_line = 0;
	tsh->line = (char *)malloc(1);
	if (!tsh->line)
		return(error_handler("memmory doesn't allocated"));
	tsh->line[0] = '\0';
	return (0);
}

int	main(int argc, char **argv, char **env)
{
	t_tsh	tsh;
	char	*tmp;

	(void)argc;
	(void)argv;
	init_shell(&tsh);
	env_to_lst(&tsh.env, env);
	// while(tsh.env) // debug
	// {
	// 	printf("%s=",((t_dict *)(tsh.env->content))->key);
	// 	printf("%s\n",((t_dict *)(tsh.env->content))->value);
	// 	tsh.env = tsh.env->next;
	// }
	while (tsh.is_running)
	{
		write(1, TSH_NAME, 27);
		tputs(save_cursor, 1, ft_putchar);
		tsh.his_ptr = tsh.his;
		while (!tsh.end_line)
		{
			tsh.is_termcap = 0;
			tsh.symbols = read(0, tsh.buf, 1024);
			tsh.buf[tsh.symbols] = '\0';
			write(1, tsh.buf, tsh.symbols);
			if (tsh.buf[tsh.symbols - 1] == '\n')
				tsh.end_line = 1;
			termcap_processor(tsh.buf, &tsh);
			if(!tsh.is_termcap)
			{
				tmp = ft_strjoin(tsh.line, tsh.buf); // refactor
				free(tsh.line);
				tsh.line = tmp;
			}
			else
				ft_bzero(tsh.buf, 1024);
			if (!ft_strcmp(tsh.line, "\4"))
				ctrl_d(&tsh);
		}
		if (ft_strcmp(tsh.line, "\n"))
		{
			line_parser(tsh);
			add_to_history(&tsh);
		}
		ft_bzero(tsh.buf, 1024);
		ft_bzero(tsh.line, ft_strlen(tsh.line));
		tsh.end_line = 0;
	}
	return (0);
}
