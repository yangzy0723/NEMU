#include <sys/time.h>
#include <stdio.h>
		struct timeval now_time;
int main()
{
	while(1)
	{
		int i = 1;
		gettimeofday(&now_time, NULL);
		printf("sec:%d, usec:%d\n",(int)now_time.tv_sec, (int)now_time.tv_usec);
		if(now_time.tv_usec % 100000 == 0)
			{
				printf("%ds passed", i);
				i++;
			}
 
	}
	 
}
