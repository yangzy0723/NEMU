#include <stdio.h>
#include <NDL.h>
int main()
{
	NDL_init();
	while(1)
	{
		printf("%d", (int)NDL_GetTicks());
	}
	 
}
