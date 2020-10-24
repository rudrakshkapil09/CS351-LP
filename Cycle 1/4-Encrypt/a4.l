%option noyywrap nodefault yylineno case-insensitive
%{
	#include<stdlib.h>
	int special(char s[]);
	void fn(char s[]);
	char p[100][100];
	int count=0;
	void fn1(char s[]);
%}

%%
[ \t\n] {printf(" ");}
[a-z]+(\'(s|t))? { fn(yytext);printf("%s",p[count-1]); }	/*placing printf in the fuction does not seem to work*/
[0-9]+ { fn1(yytext); printf("%s",p[count-1]);}
.|\n {}		/*ignore anything else*/
%%

int main(int argc,char *argv[])
{
	if(argc >1)
	{
		yyin=fopen(argv[1],"r");
	}
	else
		yyin = stdin;
	
	yylex();	/*
	for(int i=0;i<count;i++)
		printf("%s",p[i]);	*/
}

void fn1(char s[])
{
	int n=strlen(s);

	if(n%2==1)
	{
		for(int i=0;i<n/2;i++)
		{
			p[count][i]=s[n/2+1+i];
		}
		p[count][n/2]=s[n/2];
		for(int i=n/2+1;i<n;i++)
		{
			p[count][i]=s[i-n/2-1];
		}
	}
	else
	{
		for(int i=0;i<n/2;i++)
		{
			p[count][i]=s[n/2+i];
		}
		for(int i=n/2;i<n;i++)
		{
			p[count][i]=s[i-n/2];
		}	
	}
	//printf("%s",p[count]);
	count++;
}
void fn( char s[])
{

	int f=special(s);
	if(f==1)
	{
		int n=strlen(s);
		char b[n];
		p[count][n-1]=s[n-2];
		p[count][n-2]=s[n-1];
		for(int i=2;i<n;i++)
		{
			p[count][n-i-1]=s[i-2];
		}
		count++;
		
	}
	else
	{
		int n=strlen(s);
		char b[n];
		for(int i=0;i<n;i++)
		{
			int t=(int)s[i]+5;
			char ch=(char)t;
			p[count][n-i-1]=ch;
		}
		count++;
		//printf("%s",b);
		//strcmp(p[count++],b);
	}
	//printf("OK\n",p[count]);
}
int special(char s[])
{
	for(int i=0;i<strlen(s);i++)
	{
		 int t=(int)s[i];
		if(t==39)
		{
			return 1;
		}	
	}
	return 0;
}
