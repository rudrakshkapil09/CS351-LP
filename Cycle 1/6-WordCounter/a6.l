/*Word Counter*/
%option noyywrap nodefault yylineno case-insensitive
/*VIMP stuff, search for %option in pdf*/

%{
	//headers
	#include <stdio.h>
	#include <stdlib.h>
	#define length 1000

	//structs
	struct ref{
		int lineno;
		struct ref *next;
	};
	struct symbol{
		char* name;				//char
		int freq;				//frequency
		struct ref *reflist;	//references
	};

	// actual table & index
	struct symbol symtab[length];
	int ins = 0;

	// function declarations
	int lookup(char*);
	void addref(char*, int);
%}

%%
[a-z]+(\'(s|t|re|ve|m))? {addref(yytext,yylineno);/*use yylineno*/}
		/* [ \t\n] {ignore} -- > do not do this mistae, apparenly flex uses spaces for something WHEN THE %OPTION given abouve is used and you were messing it up, use the line beloew	**/
.|\n	/*for ignoring everything else*/
%%

/*Function Definitions */
//to print table at the end
void printtable()
{
	//headers
	printf("\tWORD\t|\tFREQUENCY\t|\t  LINES \n");
	printf("----------------+-----------------------+-----------------------\n");

	//for each entry
	for(int i = 0; i < ins; i++) {
		//print name and freq
		printf("%12s\t|\t%9d\t|\t",symtab[i].name,symtab[i].freq);
		struct ref *r = symtab[i].reflist;

		//print line numbers
		printf("{");
		while (r != NULL) {
			printf("%d", r->lineno);
			if (r->next)
				printf(",");
			r = r->next;
		}

		printf("}\n");
	}
}

int main(int argc, char** argv)
{
	FILE *f;
	if (argc < 2)
		f = stdin;
	else {
		f = fopen(argv[1],"r");
		if (!f)
		{
			printf("Invalid arguments. Enter the file name as first argument or no arguments to use stdin as input\n");
			return(-1);
		}
	}
	printf("<WC : ACTIVE>\n");

	// VIMP on how to load file
	yyrestart(f);
	// Note if you had multiple files then you have to do yyline = 1 here (its allowed) to reset the lineno as you are opening a new file*/
	yylex();
	fclose(f);
	printf("----------------------------------------------------------------\n");
	printf("\t\t\t[SYMBOL TABLE]\n");
	printf("----------------+-----------------------+-----------------------\n");
	printtable();
}

// lookup if the word exists - returns index if yes, otherwise -1
int lookup(char* word)
{
	//for each index
	for(int i=0;i<ins;i++)
		//compare using strcasecmp() for case insensitive comparisons
		if(strcasecmp(symtab[i].name,word) == 0)
			return i;

	//not found
	return -1;
}

// adds 'word' to table with line l
void addref(char* word, int l)
{
	// find location
	int loc = lookup(word);

	// new entry
	if (loc == -1) {
		//create symbol table entry
		symtab[ins].name = malloc(strlen(word));
		symtab[ins].freq = 1;
		strcpy(symtab[ins].name, word);

		//create list for the word
		struct ref *r = malloc(sizeof(struct ref));
		r->lineno = l;
		r->next = NULL;
		symtab[ins++].reflist = r;
	}

	//already present
	else {
		//increase count
		symtab[loc].freq++;

		//add to beginning of list
		struct ref *r = malloc(sizeof(struct ref));
		r->lineno = l;
		r->next = symtab[loc].reflist;
		symtab[loc].reflist = r;
	}
	return;
}
