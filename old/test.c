#include <unistd.h>

int main(int c, char **v, char **e)
{
	char *a[3];

	if (c != 2)
		return (1);
	a[0] = "/usr/bin/echo";
	a[1] = v[1];
	a[2] = NULL;
	execve(a[0], a, e);
}
