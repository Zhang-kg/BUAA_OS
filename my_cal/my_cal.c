

char _my_getchar();
void _my_putchar(char ch);

void my_cal() {
	char c = _my_getchar();
	unsigned long a = 0;
	
	while (c != '\n') {
		a = a * 10 + c - '0';
		c = _my_getchar();
	}
	
	if (a==0) _my_putchar('0');
	else {
		char buf[1000] = {0};
		char *p = buf;
		do{
			int tem = a % 2;
			*p++ = '0' + tem;
			a /=2;
		} while(a != 0);
		
		int length = p - buf;
		int begin = 0;
		int end = length - 1;
		while(end > begin) {
			char tem = buf[end];
			buf[end] = buf[begin];
			buf[begin] = tem;
			end--;
			begin++;
		}
		for (begin=0; begin < length; begin++){
			_my_putchar(buf[begin]);
		}	
	}
}
