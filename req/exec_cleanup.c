#include "h_minishell.h"

void	clean_pipemanager(t_pipemanager *p)
{
	pipeset_cleanup(p->pipes, p->pipe_count);
	free(p);
}

void	shell_cleanup(t_env *env)
{
	clean_ast(env->ast);
	env->ast = NULL;
	clean_shnode(&env->export);
	clean_shnode_dup(&env->env);
	rl_clear_history();
}
