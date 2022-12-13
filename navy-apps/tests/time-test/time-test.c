#include <NDL.h>
#include <stdio.h>

int main()
{
	NDL_Init(0);
	while(1)
	{
		printf("%d", (int)NDL_GetTicks());
	}
	 
}
