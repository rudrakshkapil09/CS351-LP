//Program for lexical analyzer
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//checks keywords
int isKeyword(char *buffer)
{
	//list of c language keywords
	char keywords[32][10] = {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", 
							 "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", 
							 "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef", 
							 "union", "unsigned", "void", "volatile", "while"};
	
	//compare with each word
	for (int i = 0; i < 32; i++)
		if (strcmp(keywords[i], buffer) == 0)
			return 1;
	return 0;	//not found
}

//checks valid identifier - no non-alphanum except underscore, and not starting with num
int isValidIdentifier(char *buffer, int j)
{
	if (isdigit(buffer[0]))
		return 0;
	for (int i = 0; i < j; i++)
		if (!(buffer[i] == '_' || isalnum(buffer[i])))
			return 0;
	return 1;
}

//checks integer
int isInteger(char *buffer, int j)
{
	for (int i = 0; i < j; i++)
		if (!((buffer[i] == '-' && i == 0) || isdigit(buffer[i])))
			return 0;
	return 1;
}

//checks real number
int isRealNumber(char *buffer, int j)
{
	int count = 0;	//counts decimal points
	for (int i = 0; i < j; i++) {
		if (buffer[i] == '.')
			count++;
		else if (!((buffer[i] == '-' && i == 0) || isdigit(buffer[i])))
			return 0;
	}
	return (count < 2);	//cannot have more than 2 decimal points
}


//check operators
int isOperator(char *buffer)
{
	//lists of c language operators
	char Arithmetic[5][3] = {"+", "-", "*", "/", "%"};
	char IncDec[2][3] = {"++", "--"};
	char Relational[6][3] = {"==", "!=", ">", "<", ">=", "<="};
	char Logical[3][2] = {"&&", "||", "!"};
	char Bitwise[6][2] = {">>", "<<", "~", "&", "|", "^"};
	char Assignment[11][3] = {"=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "!="};
	
	//compare with each word in each lise:
	for (int i = 0; i < 5; i++) {
		if (strcmp(Arithmetic[i], buffer) == 0) {
			printf("Arithmetic ");
			return 1;
		}
	}
	for (int i = 0; i < 2; i++) {
		if (strcmp(IncDec[i], buffer) == 0) {
			printf("Increment/Decrement ");
			return 1;
		}
	}
	for (int i = 0; i < 6; i++) {
		if (strcmp(Relational[i], buffer) == 0) {
			printf("Relational ");
			return 1;
		}
	}
	for (int i = 0; i < 3; i++) {
		if (strcmp(Logical[i], buffer) == 0) {
			printf("Logical ");
			return 1;
		}		
	}
	for (int i = 0; i < 6; i++) {
		if (strcmp(Bitwise[i], buffer) == 0) {
			printf("Bitwise ");
			return 1;
		}		
	}
	for (int i = 0; i < 11; i++) {
		if (strcmp(Assignment[i], buffer) == 0) {
			printf("Assignment ");
			return 1;
		}
	}
		
	return 0;	//not found
}

//check separator
int isSeparator(char ch)
{
	char separators[] = "[](){},;:";
	for (int i = 0; i < 9; i++) {
		if (separators[i] == ch)
			return 1;
	}
	return 0;
}

//checks string
int isString(char *buffer, int j)
{
	return (buffer[0] == '"' && buffer[j-1] == '"');
}

//MAIN
int main() 
{
	int j = 0;
	char ch, buffer[20];
	FILE *fp;
	fp = fopen("test.c", "r");
	
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(0);
	} 
	
	printf("\n------------------------\nComponents of 'test.c':\n------------------------\n");
	while ((ch = fgetc(fp)) != EOF) {
		if (isSeparator(ch))
			printf("%10c => Seperator\n", ch);
		else if ((ch == ' ' || ch == '\n' || ch == '\t') && (j != 0)) {
			buffer[j] = '\0';
			
			printf("%10s => ", buffer);
			if (isKeyword(buffer))
				printf("Keyword\n");
			else if (isInteger(buffer, j))
				printf("Integer\n");
			else if (isRealNumber(buffer, j))
				printf("Real Number\n");
			else if (isString(buffer, j))
				printf("String\n");
			else if (isOperator(buffer))
				printf("Operator\n");
			else if (isValidIdentifier(buffer, j))
				printf("Identifier\n");
			else printf("INVALID Identifier\n"); 
				
			j = 0;
		}
		else if (ch != ' ' && ch != '\n' && ch != '\t')
			buffer[j++] = ch;
	} 
}
