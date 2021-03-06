#include "minishell.h"

static	int	term_clean_up(void)
{
	tputs(restore_cursor, 1, ft_putint);
	tputs("\033[J", 1, ft_putint);
	return (0);
}

static	int	history_up(t_tsh *tsh)
{
	size_t	len;

	ft_bzero(tsh->buf, 1024);
	len = ft_strlen(tsh->line);
	term_clean_up();
	if (tsh->his_ptr && tsh->his_ptr->prev)
	{
		tsh->his_ptr = tsh->his_ptr->prev;
		len = ft_strlen(tsh->his_ptr->content);
		ft_freen((void **)&tsh->line);
		tsh->line = ft_strdup((char *)tsh->his_ptr->content);
		error_checker(!tsh->line, "memmory doesn't allocated", 0);
	}
	write(1, tsh->line, len);
	return (1);
}

static	int	history_down(t_tsh *tsh)
{
	size_t	len;

	ft_bzero(tsh->buf, 1024);
	len = ft_strlen(tsh->line);
	term_clean_up();
	if (tsh->his_ptr && tsh->his_ptr->next)
	{
		tsh->his_ptr = tsh->his_ptr->next;
		len = ft_strlen(tsh->his_ptr->content);
		if (tsh->line)
			ft_freen((void **)&tsh->line);
		tsh->line = ft_strdup((char *)tsh->his_ptr->content);
		error_checker(!tsh->line, "memmory doesn't allocated", 0);
	}
	write(1, tsh->line, len);
	return (1);
}

static	int	erase_symbol(t_tsh *tsh)
{
	struct winsize	win;
	int				len;
	int				cols;
	int				row;

	ioctl(1, TIOCGWINSZ, &win);
	cols = win.ws_col;
	ft_bzero(tsh->buf, 1024);
	len = (int)ft_strlen(tsh->line);
	row = (len + 17) / cols;
	if (row > 0 && (len + 17) % cols == 0)
	{
		tputs(tgoto(tgetstr("ch", 0), 1, cols), 1, ft_putint);
		tputs("\033[J", 1, ft_putint);
	}
	if (len > 0)
	{
		tputs(cursor_left, 1, ft_putint);
		tputs(delete_character, 1, ft_putint);
		tsh->line[len - 1] = '\0';
	}
	return (1);
}

int	termcap_processor(char *buf, t_tsh *tsh)
{
	if (!ft_strcmp(buf, "\033[A"))
		tsh->is_termcap = history_up(tsh);
	else if (!ft_strcmp(buf, "\033[B"))
		tsh->is_termcap = history_down(tsh);
	else if (!ft_strcmp(buf, "\177") || !ft_strcmp(buf, key_backspace))
		tsh->is_termcap = erase_symbol(tsh);
	else if (!ft_strcmp(buf, "\033[D"))
	{
		tputs(cursor_right, 1, ft_putint);
		tsh->is_termcap = 1;
	}
	else if (!ft_strcmp(buf, "\033[C"))
	{
		tputs(cursor_left, 1, ft_putint);
		tsh->is_termcap = 1;
	}
	return (0);
}
