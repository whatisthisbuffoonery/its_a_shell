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

typedef struct s_tok
{
	struct s_tok	*next;
	struct s_tok	*word_next;
	t_shnode		*env;		//expansion list //assert that operators are never assigned this list
	char			*str;		//stores one word, operator, or quoted section
	char			type;		//stores just first char of pre parsed string, which might be a dquote excluded from str field
	char			end_space;	//bool for whether the char after the token was whitespace
}					t_tok;

typedef struct s_pidnode
{
	pid_t				pid;
	struct s_pidnode	*next;
}						t_pidnode;

typedef struct	s_env
{
	t_shnode	*export;	//sorted
	t_shnode	*env;		//not sorted
	t_node		*tree;
	t_pidnode	*pids;
}				t_env;			//PSA empty strings can be in env list, null strings cannot

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
	t_tok			*argv; // N_CMD  – argv array (NULL-terminated) //cmd name is first item
	int				argc; //figure out later
	t_tok			*redir_op;		// mutually exclusive with argv
	t_tok			*redir_target;	//this too
	struct s_node	*redir_next;	// linked list of redirects on one cmd // again DO NOT FILL THE OTHER TWO FIELDS DIRECTLY
	struct s_node	*left;			// N_PIPE / N_AND / N_OR / N_GROUP //
	struct s_node	*right;			// unused for N_GROUP 
}					t_node;

typedef int	t_pipeset[2];

/*misc*/
void		signal_init(void);
int			rl_handle_signals(void);

void		make_word(t_tok *iter);
void		print_word(t_tok *tok);

int			ft_err(int n, char *s);
int			shell_assert(int cond, char *s);

void		merge_sort(t_shnode **head);

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
int			isname(t_tok *node);
int			isjoined(t_tok *node);
int			copy_tok(t_tok *tok);

/*checks for iscond or isbracket, do not use with subtok*/
int			ismeta(t_tok *tok);

/*env utils*/
void		shnode_append(t_shnode **dst, t_shnode *src);
t_shnode	*shnode_dup(t_shnode *src);
t_shnode	*find_env(char *str, t_shnode *list, int n);
int			env_add(t_env *env, t_shnode *src, char *dst);
int			expand_str(t_tok **tok, t_shnode *env);

/*init funcs*/
int			tok_init(char *buf, t_tok **tok);
int			node_init(t_tok **dst, char *src, int *cry);
void		env_init(t_env *dst, char **e);

/*token utils*/
void		tok_pop(t_tok **tok);
void		tok_node_append(t_tok **dst, t_tok *src);
t_tok		*subtok(t_tok **index, int (*f)(t_tok *));
t_tok		*toktrim(t_tok **list, t_tok *head, t_tok *tail);

/*idrk*/
int			counttype(t_tok *node, char c);

/*cleanup*/
void		tok_delone(t_tok *tok);
void		clean_tok(t_tok **tok);
void		clean_shnode_dup(t_shnode **shnode);
void		clean_shnode(t_shnode **shnode);
void		clean_ast(t_node *node);

/*print funcs*/
void		env_print(t_env *env);
void		shell_print(t_tok **tok, char *buf, t_env *env);
void		print_tok(t_tok **tok);
void		print_env(t_shnode *env);
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
