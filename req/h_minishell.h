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
	t_shnode		*env;		//expansion list //assert that operators are never assigned this list
	char			*str;		//stores one word, operator, or quoted section
	int				order;
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
typedef struct s_shlist
{
	t_cst			*cst;//list of commands
	struct s_shlist	*down;
	struct s_shlist	*across;
	t_cmd			*redir;//each shlist will self assign redirections whose depth is lower than itself
}					t_shlist;

int			isbracket(int c);
int			isop(int c);
int			isredir(int c);
int			iscontent(int c);
int			iscond(int c);
int			isenv(char c);
int			envname(char *s);

void		signal_init(void);
int			rl_handle_signals(void);

void		shnode_append(t_shnode **dst, t_shnode *src);
t_shnode	*shnode_dup(t_shnode *src);
t_shnode	*find_env(char *str, t_shnode *list, int n);

int			env_add(t_env *env, t_shnode *src, char *dst);
int			expand_str(t_cmd **cmd, t_shnode *env);

int			node_init(t_cmd **dst, char *src, int *cry);
void		env_init(t_env *dst, char **e);

int			ft_err(int n, char *s);
int			shell_assert(int cond, char *s);

void		merge_sort(t_shnode **head);

void		clean_cmd(t_cmd **cmd);
void		clean_shnode_dup(t_shnode **shnode);
void		clean_shnode(t_shnode **shnode);

int			ft_crutch(char *s, int n);
void		env_print(t_env *env);
void		shell_print(t_cmd **cmd, char *buf, t_env *env);

int			syntax_check(t_cmd **cmd, t_env *env, char *input);


int			ft_strcmp(char *a, char *b);
int			ft_isquote(int c);
int			ft_isspace(int c);
char		*ft_strndup(const char *src, size_t n);
void		*malloc_cond(void **dst, size_t size);
void		ft_putchar(char n);
void		ft_putstr(char *a);

#endif
