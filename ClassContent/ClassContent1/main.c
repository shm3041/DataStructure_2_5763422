#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ERROR 0
#define TRUE 1
#define FALSE 2

void trim(char* buf) {
	char* src = buf;
	char* dst = buf;

	while (*src != '\0') {
		if (!isspace((unsigned char)*src)) {
			*dst = *src;
			++dst;
		}
		src++;
	}
	*dst = '\0';
}

int check_paren(char* buf, int size) {
	int count = 0;

	for (int i = 0; i < size && buf[i] != '\0'; i++) {
		if (buf[i] == '(') count++;
		else if (buf[i] == ')') {
			count--;
			if (count < 0) return 0;
		}
	}

	return count == 0;
}

int check_root(char* buf, int size) {
	  
}

int main() {
	char buf[1000];
	fgets(buf, sizeof(buf), stdin);

	trim(buf);

	if (check_paren(buf, strlen(buf)) == 0 || check_root(buf, strlen(buf)) == 0) {
		printf("ERROR");
		return 0;
	}

	//int result; // = check_binary_tree(buf);

	//if (result == TRUE) {
	//	printf("TRUE");
	//	return 0;
	//}
	//else if (result == FALSE) {
	//	printf("FALSE");
	//	return 0;
	//}

	return 0;
}