#include "libft.h"

size_t	strlen_null(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i ++;
	return (i);
}
