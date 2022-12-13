#include <sys/time.h>
#include <stdio.h>
		struct timeval now_time;
int main()
{		int i = 1;
	while(1)
	{
		gettimeofday(&now_time, NULL);
		if(now_time.tv_sec == i)
			{
				printf("爱上悦宝的第%ds\n", i);
				i++;
			}
 
	}
	 
}
