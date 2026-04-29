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
	if (env->duped_fd[0])
		close(0);
	if (env->duped_fd[1])
		close(1);
	env->duped_fd[0] = 0;
	env->duped_fd[1] = 0;//could incluse an indicator for if stdin/stdout was closed. should I?
	rl_clear_history();
}
