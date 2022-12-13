#include <sys/time.h>
#include <stdio.h>
		struct timeval now_time;
int main()
{
	while(1)
	{
		int i = 1;
		gettimeofday(&now_time, NULL);
		if(now_time.tv_sec == i)
			{
				printf("%ds passed\n", i);
				i++;
			}
 
	}
	 
}
