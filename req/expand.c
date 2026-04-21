/* ************************************************************************** */
/*																			  */
/*														  :::	   ::::::::   */
/*	 expand.c											:+:		 :+:	:+:   */
/*													  +:+ +:+		  +:+	  */
/*	 By: achew <achew@student.42singapore.sg>		+#+  +:+	   +#+		  */
/*												  +#+#+#+#+#+	+#+			  */
/*	 Created: 2026/04/22 00:29:09 by achew			   #+#	  #+#			  */
/*	 Updated: 2026/04/22 00:40:30 by achew			  ###	########.fr		  */
/*																			  */
/* ************************************************************************** */

/*
** expand.c
**
** Expansion pipeline per word:
**	 1. Single pass: track quote state, expand $? and $VAR, strip quotes
**		— chars from expansion are appended as literal (quoted=1 in mask)
**		— so $o expanding to a* never globs, "a*" never globs
**	 2. Glob expansion — only on chars where mask[i] == 0 (bare, unquoted)
**
** Edge cases handled:
**	 echo "$c$c"   where $c = a"   → a"a"  (quotes from expansion are literal)
**	 echo $o	   where $o = a*   → a*    (glob skipped, came from expansion)
**	 echo "$o"	   where $o = a*   → a*    (quoted, no glob)
**	 echo $?					   → exit status as string
**	 echo ""					   → empty string (preserved)
**	 echo ''					   → empty string (preserved)
**	 unset VAR; echo $VAR		   → empty (removed from argv)
*/

#include <glob.h>
#include "h_minishell.h"

//extern int  g_exit_status;

/* ── env lookup ─────────────────────────────────────────────────────────── */

static char *env_get(t_shnode *env, char *name, int len)
{
	while (env)
	{
		if (ft_strncmp(env->name, name, len) == 0 && env->name[len] == '\0')
			return (env->str);
		env = env->next;
	}
	return (NULL);
}

/* ── word builder ────────────────────────────────────────────────────────── */
/*
** t_word holds the expanded string and a parallel mask.
** mask[i] == 1 means char i came from a quoted context or variable expansion
**			 → never glob-expanded
** mask[i] == 0 means char i is a bare unquoted literal → eligible for glob
*/

typedef struct s_word
{
	char	*str;
	char	*mask;	/* parallel to str: 0=bare 1=protected */
	int		len;
	int		cap;
}	t_word;

static t_word	*word_new(void)
{
	t_word	*w;

	w = ft_calloc(1, sizeof(t_word));
	if (!w)
		return (NULL);
	w->cap = 64;
	w->str	= ft_calloc(w->cap, 1);
	w->mask = ft_calloc(w->cap, 1);
	return (w);
}

static void word_free(t_word *w)
{
	if (!w)
		return ;
	free(w->str);
	free(w->mask);
	free(w);
}

static int	word_grow(t_word *w)
{
	char	*ns;
	char	*nm;

	w->cap *= 2;
	ns = ft_calloc(w->cap, 1);
	nm = ft_calloc(w->cap, 1);
	if (!ns || !nm)
		return (1);
	ft_memcpy(ns, w->str, w->len);
	ft_memcpy(nm, w->mask, w->len);
	free(w->str);
	free(w->mask);
	w->str	= ns;
	w->mask = nm;
	return (0);
}

static int	word_append(t_word *w, char c, char protected)
{
	if (w->len + 2 >= w->cap && word_grow(w))
		return (1);
	w->str[w->len]	= c;
	w->mask[w->len] = protected;
	w->len++;
	w->str[w->len]	= '\0';
	w->mask[w->len] = '\0';
	return (0);
}

static int	word_append_str(t_word *w, char *s, char protected)
{
	while (s && *s)
	{
		if (word_append(w, *s, protected))
			return (1);
		s++;
	}
	return (0);
}

/* ── variable name parsing ───────────────────────────────────────────────── */

static int	var_name_len(char *s)
{
	int i;

	if (!s || !*s)
		return (0);
	if (*s == '?')
		return (1);
	i = 0;
	while (s[i] && (ft_isalnum(s[i]) || s[i] == '_'))
		i++;
	return (i);
}

/* ── single-pass expand + unquote ───────────────────────────────────────── */
/*
** Returns a t_word with the fully expanded, unquoted string and its mask.
** was_quoted is set to 1 if the entire original token was quoted (for
** preserving empty strings: echo "" should pass "" as an empty arg).
*/

static t_word	*expand_token(char *src, t_shnode *env, int *was_quoted)
{
	t_word	*w;
	char	quote;
	char	*val;
	int		nlen;
	char	*exit_str;

	w = word_new();
	if (!w)
		return (NULL);
	quote = 0;
	*was_quoted = 0;
	while (*src)
	{
		/* enter/exit quote */
		if (!quote && (*src == '\'' || *src == '"'))
		{
			*was_quoted = 1;
			quote = *src++;
			continue ;
		}
		if (quote && *src == quote)
		{
			quote = 0;
			src++;
			continue ;
		}
		/* variable expansion — not inside single quotes */
		if (*src == '$' && quote != '\'')
		{
			src++; /* skip $ */
			if (*src == '{')
			{
				src++; /* skip { */
				nlen = 0;
				while (src[nlen] && src[nlen] != '}')
					nlen++;
				val = env_get(env, src, nlen);
				/* chars from expansion are always protected (mask=1) */
				word_append_str(w, val ? val : "", 1);
				src += nlen;
				if (*src == '}')
					src++;
			}
			else if (*src == '?')
			{
				exit_str = ft_itoa(muh_number);
				word_append_str(w, exit_str, 1);
				free(exit_str);
				src++;
			}
			else
			{
				nlen = var_name_len(src);
				if (nlen == 0)
				{
					/* bare $ with no valid name — treat as literal */
					word_append(w, '$', quote != 0);
				}
				else
				{
					val = env_get(env, src, nlen);
					word_append_str(w, val ? val : "", 1);
					src += nlen;
				}
			}
			continue ;
		}
		/* literal character — mask reflects current quote state */
		word_append(w, *src, quote != 0);
		src++;
	}
	return (w);
}

/* ── glob expansion ──────────────────────────────────────────────────────── */

static int	has_unquoted_glob(t_word *w)
{
	int i;

	i = 0;
	while (w->str[i])
	{
		if (w->str[i] == '*' && !w->mask[i])
			return (1);
		i++;
	}
	return (0);
}

/*
** Returns a NULL-terminated array of expanded strings.
** If glob finds nothing, returns the original pattern (bash behaviour).
** Caller frees the array and each string.
*/
static char **do_glob(t_word *w)
{
	glob_t	g;
	char	**result;
	int		i;

	if (glob(w->str, 0, NULL, &g) != 0 || g.gl_pathc == 0)
	{
		globfree(&g);
		result = ft_calloc(2, sizeof(char *));
		result[0] = ft_strdup(w->str);
		return (result);
	}
	result = ft_calloc(g.gl_pathc + 1, sizeof(char *));
	i = 0;
	while ((size_t)i < g.gl_pathc)
	{
		result[i] = ft_strdup(g.gl_pathv[i]);
		i++;
	}
	globfree(&g);
	return (result);
}

/* ── argv builder ────────────────────────────────────────────────────────── */
/*
** Takes one original token string, expands it, and appends the results
** (possibly multiple after globbing) to out_argv / out_argc.
** out_argv is realloced as needed.
*/

static int	expand_one_token(char *src, t_shnode *env,
								char ***out_argv, int *out_argc)
{
	t_word	*w;
	int		was_quoted;
	char	**globbed;
	int		i;
	char	**tmp;

	w = expand_token(src, env, &was_quoted);
	if (!w)
		return (1);
	/* empty expansion — drop the word unless it was explicitly quoted */
	if (w->len == 0 && !was_quoted)
	{
		word_free(w);
		return (0);
	}
	if (has_unquoted_glob(w))
		globbed = do_glob(w);
	else
	{
		globbed = ft_calloc(2, sizeof(char *));
		globbed[0] = ft_strdup(w->str);
	}
	word_free(w);
	/* splice globbed results into out_argv */
	i = 0;
	while (globbed[i])
	{
		tmp = realloc(*out_argv, sizeof(char *) * (*out_argc + 2));
		if (!tmp)
			return (1);
		*out_argv = tmp;
		(*out_argv)[(*out_argc)++] = globbed[i];
		(*out_argv)[*out_argc] = NULL;
		i++;
	}
	free(globbed);
	return (0);
}

/* ── redir target expansion ──────────────────────────────────────────────── */

static int	expand_redir(t_node *redir, t_shnode *env)
{
	t_word	*w;
	int		was_quoted;
	char	*old;

	if (!redir->redir_target)
		return (0);
	w = expand_token(redir->redir_target, env, &was_quoted);
	if (!w)
		return (1);
	old = redir->redir_target;
	redir->redir_target = ft_strdup(w->str);
	free(old);
	word_free(w);
	return (0);
}

/* ── node expansion ──────────────────────────────────────────────────────── */

static int	expand_cmd(t_node *n, t_shnode *env)
{
	char	**new_argv;
	int		new_argc;
	int		i;
	t_node	*r;

	/* rebuild argv with expansion + glob splicing */
	new_argv = ft_calloc(1, sizeof(char *));
	new_argc = 0;
	i = 0;
	while (n->argv && n->argv[i])
	{
		if (expand_one_token(n->argv[i], env, &new_argv, &new_argc))
			return (1);
		free(n->argv[i]);
		i++;
	}
	free(n->argv);
	n->argv = new_argv;
	n->argc = new_argc;
	/* expand redir targets */
	r = n->redir_next;
	while (r)
	{
		if (expand_redir(r, env))
			return (1);
		r = r->redir_next;
	}
	return (0);
}

int expand_ast(t_node *n, t_shnode *env)
{
	if (!n)
		return (0);
	if (n->kind == N_CMD)
		return (expand_cmd(n, env));
	if (n->kind == N_GROUP)
	{
		if (expand_ast(n->left, env))
			return (1);
		/* expand any redirects on the group itself */
		t_node *r = n->redir_next;
		while (r)
		{
			if (expand_redir(r, env))
				return (1);
			r = r->redir_next;
		}
		return (0);
	}
	if (n->kind == N_PIPE || n->kind == N_AND || n->kind == N_OR)
		return (expand_ast(n->left, env) || expand_ast(n->right, env));
	return (0);
}
