%option noyywrap

%{
	//Lex program to encrypt a text
	//		1) special words - rotate right twice, and reverse
	// X	2) numbers - swap left half with right half. Middle stays in place, if any
	// X	3) normal words - characters replaced with ascii + k-cyclic, and reverse
	// X	4) whitespaces - replaced with a single space
	// X	5) others - print as is

	//headers
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>

	//action function declarations
	void func1(char c[]);
	void func2(char c[]);
	void func3(char c[]);
	void func4(char c[]);
	void func5(char c[]);

	//utility function declarations
	void reverse(char c[], int s, int n);

	//globals
	int K = 4;
%}

specials	(blue|red|yellow|brown|green|purple|black|white)
whitespace	[ \t\n]+
digit 		[0-9]+
word 		[a-zA-Z]+(\'(s|t|ve|m))?

%%
{specials} 		{func1(yytext);}
{word}			{func2(yytext);}
{digit}			{func3(yytext);}
{whitespace} 	{func4(yytext);}
.				{func5(yytext);}
<<EOF>>			{printf("\n[ENCRYPTED]"); return 0;}
%%

//MAIN
int main(int args, char *argv[])
{
	//open input file if needed
	if (args > 1)
		yyin = fopen(argv[1],"r");
	else yyin = stdin;

	//open output file and make stdout to the file
	yyout = fopen("output.txt", "w");
	dup2(fileno(yyout), 1);

	//start
	yylex();
	return 0;
}

/* ACTION FUNCTION DEFINITIONS */
//for special words
void func1(char input[])
{
	int d = 2;	//how much you want to rotate by

	//find n
	int n = 0;
	for (; input[n] != '\0'; n++);

	reverse(input, 0, (n-d));
	reverse(input, (n-d), n);
	//	reverse(input, 0, n);	cancelled out by next


	//reverse and print after finding n
	//	reverse(input, 0, n);
	printf("%s", input);
}

//for words
void func2(char input[])
{
	//thhis will be output
	char newString[100];

	//for each character
	int i;
	for (i = 0; input[i] != '\0'; i++) {
		int val = (int) input[i]; 	//ASCII Value
		int temp = K;				//store duplicate

		//find the appropriate case and adjust start and end
		int start, end;
		if (val > 93) {	 //lower -> 97-1 to 122
			start = 96;
			end = 122;
		}
		else { 					//upper -> 65-1 to 90
			start = 64;
			end = 90;
		}

		//if k-th ahead character exceeds end
		if (val + K > end) {
			K -= (end - val);
			K %= 26;
			newString[i] = (char) (start + K);
		}
		else newString[i] = (char) (val + K);

		K = temp;	//restore
	}
	newString[i] = '\0';


	//reverse and print after finding n
	int n = 0;
	for (; newString[n] != '\0'; n++);
	reverse(newString, 0, n);
	printf("%s", newString);
}

//for digits
void func3(char input[])
{
	//find n
	int n = 0;
	for (; input[n] != '\0'; n++);

	//extract first half
	char first[100];
	int f = 0, i = 0;
	for (i = 0; i < n/2; i++)
		first[f++] = input[i];
	first[f] = '\0';

	//skip middle element for odd len
	if (n % 2 != 0)
		i++;

	//extract second half
	char second[100];
	int s = 0;
	for (; i < n; i++)
		second[s++] = input[i];
	second[s] = '\0';

	//get outputString
	char output[200];
	strcpy(output, second);
	if (n % 2 != 0)				//if odd
		output[s] = input[n/2];	//middle stays as it is
	strcat(output, first);
	output[i] = '\0';

	//output
	printf("%s", output);
}

//for whitespaces
void func4(char input[])
{
	printf(" ");
}

//for others
void func5(char input[])
{
	printf("%s", yytext);
}


/* UILITY FUNCTION DEFINITIONS */
//reverses a string passed to it
void reverse(char input[], int start, int n)
{
	//swap i and n-1-i
	for (int i = start; i < n/2; i++) {
		char temp = input[i];
		input[i] = input[n-1-i];
		input[n-1-i] = temp;
	}
}
