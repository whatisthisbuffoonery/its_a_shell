#ifndef H_MINISHELL_H
# define H_MINISHELL_H

# include <signal.h>
# include <stdio.h>//readline
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <errno.h>
# include "libft.h"

extern volatile sig_atomic_t	muh_number;

typedef struct s_shnode
{
	struct s_shnode	*next;
	char			*name;
	char			*str;
}					t_shnode;


typedef struct s_cmd
{
	struct s_cmd	*next;
	struct s_cmd	*word_next;
	t_shnode		*env;		//expansion list //assert that operators are never assigned this list
	char			*str;		//stores one word, operator, or quoted section
	char			type;		//stores just first char of pre parsed string, which might be a dquote excluded from str field
	char			end_space;	//bool for whether the char after the token was whitespace
}					t_cmd;

//t_list? //t_shnode? (just has str field) //self reallocing char **?
typedef struct	s_env
{
	t_shnode	*export;		//env vars corresponding to export builtin	//export list stores env variables in no particular order, but can contain items with empty strings
	t_shnode	*env;			//env vars corresponding to env builtin		//env list stores env variables in alphabet order, items with empty strings as values are never added to this list
}				t_env;			//PSA empty strings can be in env list, null strings cannot
								//
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
/*
typedef struct s_node
{
	t_node_kind		kind;
	char			**argv; // N_CMD  – argv array (NULL-terminated) 
	int				argc;
	char			*redir_op;		// N_REDIR – attached to the cmd or group it belongs to 
	char			*redir_target;
	struct s_node	*redir_next;	// linked list of redirects on one cmd 
	struct s_node	*left;			// N_PIPE / N_AND / N_OR / N_GROUP 
	struct s_node	*right;			// unused for N_GROUP 
}					t_node;
*/

typedef struct s_node
{
	t_node_kind		kind;
	t_cmd			*argv; // N_CMD  – argv array (NULL-terminated) //cmd name is first item
	int				argc; //figure out later
	t_cmd			*redir_op;		// mutually exclusive with argv
	t_cmd			*redir_target;	//this too
	struct s_node	*redir_next;	// linked list of redirects on one cmd // again DO NOT FILL THE OTHER TWO FIELDS DIRECTLY
	struct s_node	*left;			// N_PIPE / N_AND / N_OR / N_GROUP //
	struct s_node	*right;			// unused for N_GROUP 
}					t_node;

void		make_word(t_cmd *iter);
void		print_word(t_cmd *tok);

int			isbracket(int c);
int			isop(int c);
int			isredir(int c);
int			iscontent(int c);
int			iscond(int c);
int			isenv(char c);
int			envname(char *s);

int			single_cmd(t_cmd *iter);
int			isname(t_cmd *node);
int			isjoined(t_cmd *node);
int			ismeta(t_cmd *cmd);

void		signal_init(void);
int			rl_handle_signals(void);

void		shnode_append(t_shnode **dst, t_shnode *src);
t_shnode	*shnode_dup(t_shnode *src);
t_shnode	*find_env(char *str, t_shnode *list, int n);

void		cmd_node_append(t_cmd **dst, t_cmd *src);

int			env_add(t_env *env, t_shnode *src, char *dst);
int			expand_str(t_cmd **cmd, t_shnode *env);

int			cmd_init(char *buf, t_cmd **cmd);
int			node_init(t_cmd **dst, char *src, int *cry);
void		env_init(t_env *dst, char **e);
t_cmd		*subcmd(t_cmd **index, int (*f)(t_cmd *));
t_cmd		*cmdtrim(t_cmd **list, t_cmd *head, t_cmd *tail);
void		cmd_pop(t_cmd **cmd);

int			ft_err(int n, char *s);
int			shell_assert(int cond, char *s);

void		merge_sort(t_shnode **head);

int			isjoined(t_cmd *node);
int			copy_cmd(t_cmd *cmd);
int			counttype(t_cmd *node, char c);

void		cmd_delone(t_cmd *cmd);
void		clean_cmd(t_cmd **cmd);
void		clean_shnode_dup(t_shnode **shnode);
void		clean_shnode(t_shnode **shnode);
void		clean_ast(t_node *node);

int			ft_crutch(char *s, int n);
void		env_print(t_env *env);
void		shell_print(t_cmd **cmd, char *buf, t_env *env);
void		print_cmd(t_cmd **cmd);
void		print_env(t_shnode *env);
void		print_linear_cmd(t_cmd *cmd, char *s);

int			syntax_check(t_cmd **cmd, t_env *env, char *input);

t_node		*parse(t_cmd **tokens);
t_node		*parse_list(t_cmd **tok, int *stop);
t_node		*parse_pipeline(t_cmd **tok, int *stop);
t_node		*parse_command(t_cmd **tok, int *stop);
t_node		*parse_group(t_cmd **tok, int *stop);
t_node		*parse_simple_cmd(t_cmd **tok, int *stop);
t_node		*parse_redirects(t_cmd **tok, int *stop);
void		print_ast(t_node *n, int depth);

#endif
