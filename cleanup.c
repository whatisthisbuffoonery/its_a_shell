#include "h_minishell.h"

void	clean_shnode(t_shnode *env)
{
	t_shnode	*next;

	while (env)
	{
		free(env->str);
		free(env->name);
		next = env->next;
		free(env);
		env = next;
	}
}

void	clean_env_dup(t_shnode *env)
{
	t_shnode	*next;

	while (env)
	{
		next = env->next;
		free(env);
		env = next;
	}
}

void	clean_env(t_env *env)
{
	clean_shnode(env->export);
	clean_shnode_dup(env->env);
}

void	clean_cmd(t_cmd **cmd)
{
	t_cmd	*iter;
	t_cmd	*next;

	iter = *cmd;
	while(iter)
	{
		next = iter->next;
		clean_shnode_dup(iter->env);
		free(iter->str);
		free(iter);
		iter = next;
	}
}
