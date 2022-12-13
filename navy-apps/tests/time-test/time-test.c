#include <NDL.h>
#include <stdio.h>

int main()
{
	NDL_Init(0);
	int ms_500 = 500;
	while(1)
	{
		if(NDL_GetTicks() == ms_500)
		{
			printf("0.5s passed!\n");
			ms_500+=500;
		}
	}
	 
}
