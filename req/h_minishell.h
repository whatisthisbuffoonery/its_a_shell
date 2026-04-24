#ifndef H_MINISHELL_H
# define H_MINISHELL_H

# include <signal.h>
# include <stdio.h>//readline
# include <readline/readline.h>
# include <readline/history.h>
# include <sys/wait.h>
# include <errno.h>
# include "Libft/libft/libft.h"
# include "Libft/printf/ft_printf.h"
# include "Libft/gnl/get_next_line_bonus.h"

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
	int				depth;
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
}	t_node_kind;

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

typedef struct s_parser	//parser state
{
	t_cmd	*cur;	// current token 
	int		err;	// non-zero → parse failed 
}	t_parser;

typedef struct s_cst
{
	t_cmd			*cmd;	//first valid cmd string, might be a glob
	t_cmd			*args;
	t_cmd			*op;	//nearest operator to the right of cmd, really should be an enum
	t_cmd			*redir;
	t_cmd			*brackets;//just for cleanup convenience
	struct s_cst	*next;	//points to next cmd after op, take note of depth
	int				depth;
}					t_cst;

//how does this make globbing less painful than before? moar norminette space is how
//advise separating strcmp and opendir operations
//I do realise interpreting '/' is outside the subject (chdir automatically handles this)
//how we 
typedef struct s_glob
{
	t_cmd		*cmd;//for cleanup convenience
	char		*str;//the str field from cmd //use this if glob comes up with nothing
	t_shnode	*results;
	int			index;
}				t_glob;

//there was the opportunity to name this, confusingly, t_ast
typedef struct s_dlist
{
	t_cst			*cst;//list of commands
	struct s_dlist	*down;//arr? diff struct? down next?//down next.
	struct s_dlist	*down_next;
	struct s_dlist	*across;
	t_cmd			*redir;//each shlist will self assign redirections whose depth is lower than itself
}					t_dlist;

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
int			hascommand(t_cst *cst);
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
t_cst		*cst_init(t_cmd **cmd, int *complain, int depth, t_cmd *op);
t_cmd		*subcmd(t_cmd **index, int (*f)(t_cmd *));
t_cmd		*cmdtrim(t_cmd **list, t_cmd *head, t_cmd *tail);
void		cmd_pop(t_cmd **cmd);
t_dlist		*dlist_init(t_cst **cst, int *complain, int depth, t_cmd **redir);
int			issubshell(t_cst **cst, char type);

int			ft_err(int n, char *s);
int			shell_assert(int cond, char *s);
t_cst		*cst_complain(int *complain, t_cst *cst, char *s);
t_dlist		*dlist_complain(int *complain, t_dlist *dlist, char *s);

void		merge_sort(t_shnode **head);

int			isjoined(t_cmd *node);
int			copy_cmd(t_cmd *cmd);
int			counttype(t_cmd *node, char c);

t_cst		*cst_pop(t_cst **cst);

void		cmd_delone(t_cmd *cmd);
void		clean_cmd(t_cmd **cmd);
void		clean_shnode_dup(t_shnode **shnode);
void		clean_shnode(t_shnode **shnode);
void		clean_cst(t_cst **cst);
void		clean_dlist(t_dlist *dlist);

int			ft_crutch(char *s, int n);
void		env_print(t_env *env);
void		shell_print(t_cmd **cmd, char *buf, t_env *env);
void		print_cmd(t_cmd **cmd);
void		print_env(t_shnode *env);
void		print_cst(t_cst *cst, int check);
void		print_linear_cst(t_cst *cst);
void		print_dlist(t_dlist *dlist, int depth);
void		print_linear_cmd(t_cmd *cmd, char *s);
void		print_dlist_digestible(t_dlist *dlist);

int			syntax_check(t_cmd **cmd, t_env *env, char *input);

t_node		*parse(t_cmd *tokens);
t_node		*parse_list(t_parser *p);
t_node		*parse_pipeline(t_parser *p);
t_node		*parse_command(t_parser *p);
t_node		*parse_group(t_parser *p);
t_node		*parse_simple_cmd(t_parser *p);
t_node		*parse_redirects(t_parser *p);
void		ast_print(t_node *n, int depth);

int			ft_strcmp(char *a, char *b);
int			ft_isquote(int c);
int			ft_isspace(int c);
char		*ft_strndup(const char *src, size_t n);
void		*malloc_cond(void **dst, size_t size);
void		ft_putchar(char n);
void		ft_putstr(char *a);

#endif
