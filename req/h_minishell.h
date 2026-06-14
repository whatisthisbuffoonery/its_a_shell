#ifndef H_MINISHELL_H
# define H_MINISHELL_H

# include <signal.h>
# include <stdio.h>//readline
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include "libft.h"

extern volatile sig_atomic_t	signo;

typedef struct s_shnode
{
	struct s_shnode	*next;
	char			*name;
	char			*str;
}					t_shnode;

typedef struct s_arg
{
	char			*str;
	char			*mask;
	int				assignment;
	struct s_arg	*next;
}					t_arg;

typedef struct s_tok
{
	struct s_tok	*next;
	struct s_tok	*word_next;
	char			*str;		//stores one word, operator, or quoted section
	char			type;		//stores just first char of pre parsed string, which might be a dquote excluded from str field
	char			end_space;	//bool for whether the char after the token was whitespace
	int				assignment;
}					t_tok;

typedef enum e_node_kind
{
	N_CMD,		//simple command		  
	N_PIPE,		//pipeline  left | right
	N_AND,		//left && right 
	N_OR,		//left || right
	N_GROUP,	//( inner )	  
	N_REDIR,	//redirection
	N_ILLEGAL,
}	t_node_kind;

typedef struct s_node
{
	t_node_kind		kind;
	t_tok			*argv;			// N_CMD  – argv array (NULL-terminated) //cmd name is first item
	t_tok			*redir_op;		// mutually exclusive with argv
	t_tok			*redir_target;	//this too
	struct s_node	*redir_next;	// linked list of redirects on one cmd // again DO NOT FILL THE OTHER TWO FIELDS DIRECTLY
	struct s_node	*left;			// N_PIPE / N_AND / N_OR / N_GROUP //
	struct s_node	*right;			// unused for N_GROUP
	int				heredoc;		//for expanding heredoc contents
}					t_node;

typedef int	t_pipeset[2];

typedef struct	s_glob
{
	char			**glob_arr;
	struct s_glob	*next;
}					t_glob;

typedef struct s_pipemanager
{
	size_t					pipe_count;
	size_t					pid_count;
	pid_t					pid;//just store the last child
	t_pipeset				*pipes;
}							t_pipemanager;

typedef struct	s_env
{
	t_shnode		*export;	//not sorted
	t_shnode		*env;		//also not sorted
	t_node			*ast;
	char			*pwd;
	char			*oldpwd;			//these will be separate strings from the env lists //to be handled in env_init
	int				duped_fd[2];
	char			last_string[4];
	char			do_not_subshell;
	char			is_in_subshell;
	char			last;
}					t_env;				//PSA empty strings can be in env list, null strings cannot

/*signal handling*/
void		signal_init(void);
int			rl_handle_signals(void);

/*word funcs*/
void		make_word(t_tok *iter);
void		print_word(t_tok *tok);
char		*word_to_str(char **dst, t_tok *src);

void		merge_sort(t_shnode **head);

int			isempty(char *buf);

int			child_wait(pid_t pid);
int			pipe_dup(int *fd);

void		update_last(t_env *env, int n);

/*error printing*/
int			ft_err(int n, char *s);
int			shell_assert(int cond, char *s);
int			shell_assert2(int cond, char *name, char *s);
int			shell_assert_redir(int cond, t_tok *iter, char *s);
int			builtin_err(int cond, char *name, char *s);

/*env updating*/
int			update_shell_lvl(t_env *dst, int is_subshell);
int			update_shell_name(t_env *dst);

/*glob utils*/
int			has_glob(t_arg *field);
int			do_glob(t_arg **prev, t_arg **iter, t_arg **next, t_arg **head);

/*basic type checking*/
int			isbracket(int c);
int			isop(int c);
int			isredir(int c);
int			iscontent(int c);
int			iscond(int c);
int			isenv(char c);
int			envname(char *s);

/*node checking, can be used with subtok*/
int			single_tok(t_tok *iter);
int			isjoined(t_tok *node);
int			copy_tok(t_tok *tok);

/*builtin stuff*/
int			do_builtin_match(int argc, char **argv, t_env *env, int *fd);
int			echo(char **argv, int out);
int			cd(int argc, char **argv, t_env *env);
int			pwd(t_env *env, int out);
int			env_builtin(int argc, char **argv, t_env *env, int out);
int			ft_export(int argc, char **argv, t_env *env, int out);
int			exit_builtin(int argc, char **argv, t_env *env, int *fd);
int			unset_builtin(int argc, char **argv, t_env *env);

/*cd utils*/
int			new_pwd(char *pwd, char *v);

/*checks for iscond or isbracket, do not use with subtok*/
int			ismeta(t_tok *tok);

/*other checkers*/
int			isbuiltin(char *s);
int			isname(t_tok *node);
int			is_assignment_word(char *s);

/*process helpers*/
pid_t		shell_fork(t_env *env);
void		pid_bump(t_pipemanager *p, pid_t src);

/*expansion things*/
char		*find_env_str(char *name, t_env *env, unsigned int len);
int			expand_all_debug(t_tok **tok, t_env *env);
int			expand_str(t_tok *tok, t_env *env);

char		**expand_all(t_tok *src, t_env *env, int (*f)(t_arg **, t_tok *));

/*iterators for expand_all*/
int			collect_argv(t_arg **dst, t_tok *src);
int			collect_redir(t_arg **dst, t_tok *src);

/*expand_all callers*/
char		**make_envp(t_env *env, int *complain);
char    	**make_argv(t_tok *src, t_env *env);
int			redir_to_fd(t_node *node, t_env *env, int *fd, int *pfd);

/*env utils*/
t_shnode	*env_init_node(char *e);
void		shnode_append(t_shnode **dst, t_shnode *src);
t_shnode	*shnode_dup(t_shnode *src);
t_shnode	*find_env(char *str, t_shnode *list);
int			env_add(t_env *env, t_shnode *src, char *dst);
int			use_expansion(t_tok *dst, t_env *env, char *ret);
int			split_expand(t_arg **dst, t_tok *src);

t_arg		*fake_token(void);

t_arg		*append_new_field(t_arg *new, t_arg **head, t_arg **cur);
t_arg		*field_split(t_arg *src);
void		free_arg(t_arg *arg);
t_arg		*free_arg_list(t_arg *head);
t_arg		*expand_globs(t_arg *fields);

char		*grab_home(t_env *env);

/*init funcs*/
int			tok_init(char *buf, t_tok **tok);
int			node_init(t_tok **dst, char *src, int *cry);
void		env_init(t_env *dst, char **e);
void		pipemanager_init(t_pipemanager *dst, int p_index);

/*token utils*/
void		tok_pop(t_tok **tok);
void		tok_node_append(t_tok **dst, t_tok *src);
t_tok		*subtok(t_tok **index, int (*f)(t_tok *));
t_tok		*toktrim(t_tok **list, t_tok *head, t_tok *tail);

/*idrk*/
int			counttype(t_tok *node, char c);

/*execution entry point*/
int			do_list(t_node *node, t_env *env);

/*cleanup*/
void		tok_delone(t_tok *tok);
void		clean_tok(t_tok **tok);
void		clean_shnode_dup(t_shnode **shnode);
void		clean_shnode(t_shnode **shnode);
void		*clean_one_shnode(t_shnode *node);
void		clean_ast(t_node *node);
void		clean_pipemanager(t_pipemanager *p);
void		shell_cleanup(t_env *env);
void		unset(int *fd);
void		pipeset_cleanup(t_pipeset *set, size_t n);

/*print funcs*/
void		env_print_debug(t_env *env);
void		shell_print(t_tok **tok, char *buf, t_env *env);
void		print_tok(t_tok **tok);
void		print_env_debug(t_shnode *env);
void		print_env(t_shnode *env, int fd);
void		print_linear_tok(t_tok *tok, char *s);
void		print_ast(t_node *n, int depth);

/*ast funcs*/
t_node		*parse(t_tok **tokens);
t_node		*parse_list(t_tok **tok, int *stop);
t_node		*parse_pipeline(t_tok **tok, int *stop);
t_node		*parse_command(t_tok **tok, int *stop);
t_node		*parse_group(t_tok **tok, int *stop);
t_node		*parse_simple_cmd(t_tok **tok, int *stop);

t_node		*parse_redir_group(t_tok **tok, int *stop);
t_node		*parse_one_redir(t_tok **tok, int *stop);
void		redir_append(t_node *dst, t_node *src);

int			isarg(char c);
int			ast_iscond(t_tok *tok);
t_node		*node_new(t_node_kind kind, int *complain);
t_node_kind	find_kind_op(t_tok *tok);

#endif
