#include <unistd.h>
#include <stdlib.h>

int main(void)
{
	char *a[2];

	a[0] = "/usr/bin/echo";
	a[1] = NULL;
	execve(a[0], a, NULL);
}
