#include "minishell.h"

static int	clear_parser(t_tsh *tsh, char **current_line)
{
	free(tsh->prsr.line);
	if (current_line)
		free(*current_line);
	clear_redirects(tsh);
	clear_arr(&tsh->prsr.args);
	return (1);
}

static void	init_parser(t_tsh *tsh)
{
	tsh->prsr.args = (char **)malloc(sizeof(char *) * 2);
	error_checker(!tsh->prsr.args, "memmory doesn't allocated", 1);
	tsh->prsr.args[0] = (char *)malloc(1);
	error_checker(!tsh->prsr.args[0], "memmory doesn't allocated", 1);
	tsh->prsr.args[0][0] = '\0';
	tsh->prsr.args[1] = NULL;
	tsh->prsr.redirects = (t_redrct **)malloc(sizeof(t_redrct *));
	error_checker(!tsh->prsr.redirects, "memmory doesn't allocated", 1);
	tsh->prsr.redirects[0] = NULL;
	tsh->prsr.current_arg = 0;
	tsh->prsr.parse_status = 1;
}

static void	pipes_redirects(t_tsh *tsh, char **current_line)
{
	if (tsh->prsr.parse_status == 2)
	{
		tsh->prsr.parse_status = 1;
		redirect_handler(tsh);
		pipe_processor(tsh);
		if (tsh->prsr.parse_status == 0)
			return ;
		clear_redirects(tsh);
		clear_arr(&tsh->prsr.args);
		init_parser(tsh);
	}
	if (tsh->prsr.parse_status >= 3)
	{
		redirect_handler(tsh);
		cmd_processor(tsh);
		tsh->prsr.parse_status = 1;
		clear_parser(tsh, NULL);
		init_parser(tsh);
		if (!(*current_line)[skip_whitespaces(*current_line, 1)] ||
		(*current_line)[skip_whitespaces(*current_line, 1)] == '\n' )
			ft_freen((void **)&tsh->prsr.args[0]);
		tsh->prsr.line = preparser(current_line, tsh);
		tsh->prsr.l_index = -1;
	}
}

static void	ending(t_tsh *tsh, char **current_line)
{
	redirect_handler(tsh);
	free(tsh->prsr.line);
	free(*current_line);
	if (!tsh->prsr.pipe.count)
		cmd_processor(tsh);
	else
		wait_pipes(tsh);
	clear_redirects(tsh);
	clear_arr(&tsh->prsr.args);
	tsh->prsr.parse_status = 0;
}

void	line_parser(t_tsh *tsh)
{
	char	*current_line;

	current_line = ft_strdup(tsh->line);
	tsh->prsr.l_index = 0;
	init_parser(tsh);
	syntax_checker(tsh);
	tsh->prsr.line = preparser(&current_line, tsh);
	if (!tsh->prsr.parse_status && clear_parser(tsh, &current_line))
		return ;
	while (tsh->prsr.line[tsh->prsr.l_index] && \
	tsh->prsr.line[tsh->prsr.l_index] != '\n')
	{
		if (!tsh->prsr.parse_status && clear_parser(tsh, &current_line))
			return ;
		distributor(&tsh->prsr);
		if (tsh->prsr.l_index >= (int)ft_strlen(tsh->prsr.line) || \
		!tsh->prsr.parse_status || tsh->prsr.line[tsh->prsr.l_index] == '\n')
			break ;
		pipes_redirects(tsh, &current_line);
		tsh->prsr.l_index++;
	}
	ending(tsh, &current_line);
}
