#include "minishell.h"

static int	open_processor(t_tsh *tsh, int i)
{
	if (tsh->input_fd)
		close(tsh->input_fd);
	if (tsh->output_fd)
		close(tsh->output_fd);
	if (tsh->prsr.redirects[i]->type == 3)
		tsh->input_fd = open(tsh->prsr.redirects[i]->file_path, O_RDONLY);
	if (tsh->prsr.redirects[i]->type == 2)
		tsh->output_fd = open(tsh->prsr.redirects[i]->file_path, \
		O_CREAT | O_RDWR | O_APPEND, 0755);
	if (tsh->prsr.redirects[i]->type == 1)
		tsh->output_fd = open(tsh->prsr.redirects[i]->file_path, \
		O_CREAT | O_RDWR | O_TRUNC, 0755);
	if (tsh->output_fd < 0 || tsh->input_fd < 0)
	{
		ft_putstr_fd("turboshell-1.0: ", 2);
		ft_putstr_fd(tsh->prsr.redirects[i]->file_path, 2);
		write(2, ": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		tsh->prsr.parse_status = -1;
		tsh->output_fd = 0;
		tsh->input_fd = 0;
		g_exit_status = 1;
		return (0);
	}
	return (1);
}

void	open_redirects(t_tsh *tsh)
{
	int	i;

	i = -1;
	while (tsh->prsr.redirects[++i])
		if (!open_processor(tsh, i))
			return ;
	if (i)
	{
		if (tsh->input_fd)
			dup2(tsh->input_fd, 0);
		if (tsh->output_fd)
			dup2(tsh->output_fd, 1);
	}
}

void	close_redirects(t_tsh *tsh)
{
	if (tsh->prsr.redirects[0])
	{
		if (tsh->input_fd)
		{
			dup2(tsh->original_fd[0], 0);
			close(tsh->input_fd);
			tsh->input_fd = 0;
		}
		if (tsh->output_fd)
		{
			dup2(tsh->original_fd[1], 1);
			close(tsh->output_fd);
			tsh->output_fd = 0;
		}
	}
}

static void	redirects_set_path(t_tsh *tsh)
{
	int	i;

	i = -1;
	while (tsh->prsr.redirects[++i])
	{
		if (!tsh->prsr.redirects[i]->file_path)
		{
			tsh->prsr.redirects[i]->file_path = \
			ft_strdup(tsh->prsr.args[tsh->prsr.redirects[i]->arg_num]);
			error_checker(!tsh->prsr.redirects[i]->file_path, \
			"memmory doesn't allocated", 1);
			ft_freen((void **)&tsh->prsr.args[tsh->prsr.redirects[i]->arg_num]);
			tsh->prsr.args[tsh->prsr.redirects[i]->arg_num] = ft_strdup("\n");
			error_checker(!tsh->prsr.args[tsh->prsr.redirects[i]->arg_num], \
			"memmory doesn't allocated", 1);
		}
	}
}

void	redirect_handler(t_tsh *tsh)
{
	char	**res_arr;
	int		len;
	int		i;

	redirects_set_path(tsh);
	i = -1;
	len = 0;
	while (tsh->prsr.args[++i])
		if (tsh->prsr.args[i][0] != '\n')
			len++;
	res_arr = (char **)malloc(sizeof(char *) * (len + 1));
	error_checker(!res_arr, "memmory doesn't allocated", 1);
	res_arr[len] = NULL;
	i = -1;
	len = -1;
	while (tsh->prsr.args[++i])
	{
		if (tsh->prsr.args[i][0] != '\n')
		{
			res_arr[++len] = ft_strdup(tsh->prsr.args[i]);
			error_checker(!res_arr[len], "memmory doesn't allocated", 1);
		}
	}
	clear_arr(&tsh->prsr.args);
	tsh->prsr.args = res_arr;
}
