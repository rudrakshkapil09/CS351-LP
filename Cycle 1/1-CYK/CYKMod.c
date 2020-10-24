// CYK Algorithm with Derivation Sequence Printed
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#define  MAX 15

//structs
struct cell
{
	char V[10];
	int count;
};

struct rule
{
	char L;
	char R[5];
};

//globals
struct rule G[20];
struct cell C[MAX][MAX];
char seq[20][200];
int scount = 0;
char str[MAX];
char startVar = -1;
int n = 0;
int L;

//print cells of table
void printCells()
{
	for (int i = 0; i < L; i++) {
		for (int j = 0; j < L-i; j++)
			printf("%6s  |", C[i][j].V);
		printf("\n");
		for (int j = 0; j < L-i; j++)	//only for printing lines
			printf("--------+");
		printf("\n");
	}
}

//takes grammar as input and stores in global arrays
void readGrammar()
{
	printf("\n\n-------------------CYK Algorithm-------------------\nEnter Grammar:\nFirst production is starting\n");
	printf("First letter is LHS, rest are RHS\n(Exit with 0)\n");
	while (1) {
		//get input
		char inp[5];
		printf("[%2d]: ", (n+1));
		scanf("%s", inp);

		//check exit and validity
		if (inp[0] == '0')
			break;

		if (strlen(inp) < 2) {
			printf("Invalid input!\n");
			continue;
		}

		//for first time, get starting variable
		if (startVar == -1)
			startVar = inp[0];

		//valid rule - store in array
		G[n].L = inp[0];
		int i = 1;
		for (; i < strlen(inp); i++)
			G[n].R[i-1] = inp[i];
		G[n].R[i-1] = '\0';
		n++;
	}
}

//prints grammar
void printGrammar()
{
	printf("Grammar => [\n");
	for (int i = 0; i < n; i++)
		printf("%c -> %s\n", G[i].L, G[i].R);
	printf("]\n\n");
}

//stores Vars producing s in t
void getVar(char s[], struct cell *t)
{
	// for each production
	for (int i = 0; i < n; i++) {
		// if RHS matches given s
		if (strcmp(G[i].R, s) == 0) {
			//some times the given symbol is already present so check before you add
			int present = 0;
			for (int k = 0; k < t->count; k++) {
				if (t->V[k] == G[i].L) {
					//printf("PRESEnt");
					present = 1;
					break;
				}
			}

			//if not already present add LHS to cell t's Vars
			if (present == 0)
				t->V[t->count++] = G[i].L;
		}
	}
	//ending delimiter
	t->V[t->count] = '\0';
}

// find first row
void firstPass()
{
	//print start of table
	printf("\n[TABLE]:\n");

	//calculate
	for (int i = 0; i < strlen(str); i++) {
		//convert str[i] to string of length 1
		char s[2];
		s[0] = str[i];
		s[1] = '\0';

		//store all vars producing str[i] in first row of table C
		getVar(s, &C[0][i]);

		//print str[i]
		printf("%6c   ", str[i]);
	}

	//for printing line below
	printf("\n");
	for (int i = 0; i < strlen(str); i++)
		printf("========+");
	printf("\n");
}

//computes value of each cell - all dependent on algo
void computeCell(int i, int j)
{
	for (int p = 0; p < i; p++) {
		// algorithm gives these equations
		int q = i-p-1;
		int r = j+p+1;

		for (int k = 0; k < strlen(C[p][j].V); k++) {
			for (int l = 0; l < strlen(C[q][r].V); l++) {
				char temp[3];
				temp[0] = C[p][j].V[k];
				temp[1] = C[q][r].V[l];
				temp[2] = '\0';

				getVar(temp, &C[i][j]);
			}
		}
	}
}

//computes cell values for all cells
void cykAlgo()
{
	//for each cell call the above function
	for (int i = 1; i < strlen(str); i++) 	//row 0 already done in firstPass()
		for (int j = 0; j < strlen(str) - i; j++)
			computeCell(i,j);
}

//function to check answer
void checkAnswer()
{
	//startVar must be present in tip of triangle
	int correct = 0;
	for (int i = 0; i < strlen(C[L-1][0].V); i++)
		if (C[L-1][0].V[i] == startVar)
			correct = 1;

	//print
	if (correct)
		printf("\n========String can be generated!=========\n");
	else printf("\nxxxxxxxx Cannot be generated xxxxxxxxxxxx\n");
}

//recursive function for printing sequences
int totalCount = 0;
void DSeq(char* ans, char *curr)
{
	//break condition
	if (strlen(curr) > L)
		return;

	// new answer:
	char nans[200];
	if (ans != NULL)
		sprintf(nans,"%s -> %s",ans,curr);	//set as ans -> curr
	else sprintf(nans,"%s",curr);			//set as curr (first time)

	//if length of curr is equal to string's - THIS IF NOT NEEDED
	if (strlen(curr) == L) {
		//if curr is same length as str print ans->curr - THIS NEEDED
		if (strcmp(curr,str) == 0) {
			printf("[%2d]: %s -> %s\n",totalCount++,ans,curr);
			return;
		}
	}

	//for each char i in curr
	for (int i = 0; i < strlen(curr); i++) {
		//for each production J in the grammar
		for (int j = 0; j < n; j++) {
			//if i is LHS of J
			if (curr[i] == G[j].L) {
				char temp[100];
				int p = 0;

				/*replace with that production*/
				//for each char k in curr
				for (int k = 0; k < strlen(curr); k++) {
					//if k is same as i then put RHS of J in temp
					if (k == i) {
						for (int l = 0; l < strlen(G[j].R); l++)
							temp[p++] = G[j].R[l];
						temp[p] = '\0';
						continue;
					}

					//otherwise just put k in temp
					temp[p++] = curr[k];
					temp[p] = '\0';
				}

				//recur with new answer as ans and temp as curr
				DSeq(nans,temp);
			}
		}
	}
}

//print sequence:
void printDerivationSequences()
{
	//check if we can even derive - INCORRECT HERE
	if (strlen(C[L-1][0].V) == 0)
		return;

	//print all possible derivation sequences:
	printf("\n\n[Possible derivation sequences]:\n");
	for(int i = 0; i < strlen(C[L-1][0].V); i++) {	//for all Vars W in tip,
		char temp[2];
		temp[0] = C[L-1][0].V[i];
		temp[1] = '\0';

		//call DSeq with answer as NULL and current as W
		DSeq(NULL, temp);
	}

	printf("\nTotal Sequences => %d\n", (totalCount-1));
	printf("================================================\n");
}

int main()
{
	//get grammar
	readGrammar();
	if (n == 0)
		return 0;
	printGrammar();

	//get string
	printf("Input the string to test: ");
	scanf("%s",str);
	L = strlen(str);

	//function calls
	firstPass();
	cykAlgo();
	printCells();
	checkAnswer();
	printDerivationSequences();
	return 0;
}
