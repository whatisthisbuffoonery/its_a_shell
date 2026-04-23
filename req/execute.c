/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achew <achew@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/22 20:04:09 by achew             #+#    #+#             */
/*   Updated: 2026/04/23 19:46:52 by achew            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* builtin dispatch table */

#include "h_minishell.h"

typedef int (*t_builtin)(int argc, char **argv, t_env *env);

typedef struct s_builtin_entry
{
    char        *name;
    t_builtin   fn;
}   t_builtin_entry;

static t_builtin_entry  g_builtins[] = {
//    {"echo",    ft_echo},
//    {"cd",      ft_cd},
//    {"pwd",     ft_pwd},
    {"export",  ft_export},
//    {"unset",   ft_unset},
//    {"env",     ft_env},
//    {"exit",    ft_exit},
//    {NULL,      NULL}
};

static t_builtin    get_builtin(char *name)
{
    int i;

    i = 0;
    while (g_builtins[i].name)
    {
        if (ft_strcmp(g_builtins[i].name, name) == 0)
            return (g_builtins[i].fn);
        i++;
    }
    return (NULL);
}

int exec_cmd(t_node *n, t_env *env)
{
    t_builtin   builtin;
    pid_t       pid;
    int         status;
	char		**envp;

    if (!n->argv || !n->argv[0])
        return (0);
//    apply_redirects(n->redir_next);  /* dup2 for redirections 
    builtin = get_builtin(n->argv[0]);
    if (builtin)
        return (builtin(n->argc, n->argv, env));  /* run in current process */
    pid = fork();
    if (pid == 0)
    {
		envp = env_to_envp(env->export);
		if (!envp)
			return (-1);
        execve(n->argv[0], n->argv, envp);  /* child */
		free_split(envp);
        perror(n->argv[0]);
        exit(127);
    }
    waitpid(pid, &status, 0);
    return (WEXITSTATUS(status));
}

int exec_group(t_node *n, t_env *env)
{
    pid_t   pid;
    int     status;

    pid = fork();
    if (pid == 0)
    {
//        apply_redirects(n->redir_next);
        exit(execute(n->left, env));
    }
    waitpid(pid, &status, 0);
    return (WEXITSTATUS(status));
}

int execute(t_node *n, t_env *env)
{
    if (!n)
        return (0);
    if (n->kind == N_CMD)
        return (exec_cmd(n, env));
//    if (n->kind == N_PIPE)
//        return (exec_pipe(n, env));
//    if (n->kind == N_AND)
//    {
//        if (execute(n->left, env) == 0)
//            return (execute(n->right, env));
//        return (1);
//    }
//    if (n->kind == N_OR)
//    {
//        if (execute(n->left, env) != 0)
//            return (execute(n->right, env));
//        return (0);
//    }
//    if (n->kind == N_GROUP)
//        return (exec_group(n, env));
    return (0);
}
