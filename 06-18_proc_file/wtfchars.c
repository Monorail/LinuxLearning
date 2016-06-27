#include <stdio.h>

int main(){
	char str[] = "beyblades";
	printf("%s\n", str);
	str[0] = 'g';
	printf("%s\n", str);
	return 0;
}