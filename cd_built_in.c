#include "minishell.h"

void	ft_cd(t_tsh *tsh, char *dir)
{
	t_list	*current;
	t_dict	*pwd;
	t_dict	*old_pwd;

	current = tsh->env;
	old_pwd = NULL;
	if (!dir)
	{
		while (current)
		{
			if (!ft_strcmp("HOME", ((t_dict *)(current->content))->key))
				dir = ((t_dict *)(current->content))->value;
			current = current->next;
		}
	}
	current = tsh->env;
	if (chdir(dir) == -1)
	{
		ft_putstr_fd("turboshell: cd:", 1);
		ft_putstr_fd(dir, 1);
		ft_putendl_fd(": No such file or directory", 1);
		return ;
	}
	while (current)
	{
		if (!ft_strcmp("PWD", ((t_dict *)(current->content))->key))
			pwd = (t_dict *)(current->content);
		if (!ft_strcmp("OLDPWD", ((t_dict *)(current->content))->key))
			old_pwd = (t_dict *)(current->content);
		current = current->next;
	}
	if (!old_pwd->value)
	{
		old_pwd->value = ft_strjoin("OLDPWD=", pwd->value);
		error_checker(!old_pwd->value, "memmory doesn't allocated", 1);
		elem_to_lst(old_pwd->value, &tsh->env);
	}
	else
	{
		ft_freen ((void **)&old_pwd->value);
		old_pwd->value = ft_strdup(pwd->value);
		error_checker(!old_pwd->value, "memmory doesn't allocated", 1);
	}
	if (pwd->value)
		ft_freen ((void **)&pwd->value);
	pwd->value = ft_strdup(dir);
	error_checker(!pwd, "memmory doesn't allocated", 1);
}