#include<stdio.h>
int main()
{
	int n;
	scanf("%d",&n);
	char a[10]={0};
	int i = 0;
	while(n){
		int x = n % 10;
		n /= 10;
		a[i] = x + '0';
		i ++;
	}
	i--;
	int j = 0;
	int flag = 0;
	while (i > j) {
		if (a[i] != a[j]) {
			flag = 1;
		}
		i--;
		j++;	
	}








	if(flag == 0){
		printf("Y");
	}else{
		printf("N");
	}
	return 0;
}
