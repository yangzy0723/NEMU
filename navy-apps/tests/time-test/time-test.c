#include <sys/time.h>
#include <stdio.h>
#include <NDL.h>
int main()
{		int i = 1;
	while(1)
	{
				printf("%d", (int)NDL_GetTicks());
				i++;
 
	}
	 
}
