#include <unistd.h>

void	ft_putstr(char *s)
{
	int i = 0;

	while (s[i])
		i ++;
	write(1, s, i);
}

int main(int c, char **v)
{
	if (c < 2)
		return (1);

	int i = 1;
	while (v[i])
	{
		ft_putstr("arg[");
		ft_putstr(v[i]);
		ft_putstr("]\n");
		i ++;
	}
}
