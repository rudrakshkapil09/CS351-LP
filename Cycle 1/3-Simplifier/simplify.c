// Program to remove useless symbols,
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#define MAX 15

// struct for holding productions
struct rule
{
	char L;
	char R[5];
	int present;	// 0 -> removed
};
struct rule G[40];

// struct for Left Recursion Rule
struct LRRule
{
	char L[3];
	char R[50];
};
struct LRRule ans[60];

// globals
char symbols[40];
int status[40];
int n, sc = 0, L, nl;
int zcounter = 1;

// function declarations
void readGrammar();
void printGrammar();
void printSymbols();
void removeUselessSymbols();
void removeUnitProductions();
void removeNullProductions();
void removeLeftRecursion();

// MAIN
int main()
{
	readGrammar();
	if (n == 0)
		return 0;

	printGrammar();
	printSymbols();

	removeUselessSymbols();
	removeNullProductions();
	removeUnitProductions();
	removeLeftRecursion();
	return 0;
}

/* function definitions */

//takes grammar as input and stores in global arrays
void readGrammar()
{
	printf("\n\n-------------------CFG-SIMPLIFIER-------------------\nEnter Grammar\nFirst production is starting\n");
	printf("First letter is LHS, rest are RHS\n(Exit with 0)\n");
	while (1) {
		//get input
		char inp[5];
		scanf("%s", inp);

		//check exit and validity
		if (inp[0] == '0')
			break;

		if (strlen(inp) < 2) {
			printf("Invalid input!\n");
			continue;
		}

		//valid rule - store in array
		G[n].L = inp[0];
		G[n].present = 1;
		int i = 1;
		for (; i < strlen(inp); i++)
			G[n].R[i-1] = inp[i];
		G[n].R[i-1] = '\0';
		n++;

		//add to symbols if not already present
		for (int k = 0; k < strlen(inp); k++) {
			int found = 0;
			for (int j = 0; j < sc; j++) {
				if (symbols[j] == inp[k]) {
					found = 1;
					break;
				}
			}

			if (found == 0) {
				symbols[sc++] = inp[k];
				symbols[sc] = '\0';
			}
		}

	}
	printf("Grammar stored!\n\n");
}

//prints grammar
void printGrammar()
{
	printf("Grammar => [\n");
	for (int i = 0; i < n; i++) {
		if (G[i].present == 0)
			continue;
		else printf("%c -> %s\n", G[i].L, G[i].R);
	}
	printf("]\n\n");
}

//prints symbols in order they originally appeared
void printSymbols()
{
	printf("Present Symbols => [%s]\n", symbols);
}

//returns index of given char in symbols array
int getSymbolIndex(char c)
{
	for (int i = 0; i < sc; i++)
		if (symbols[i] == c)
			return i;
	return -1;	//not found
}

//check if given char is reachable
int isReachable(char c)
{
	for (int i = 0; i < sc; i++)
		if (c == symbols[i] && status[i] == 2)
			return 1;
	return 0;
}

//checks if a given rule has only terminals
int isTerminable(struct rule *c)
{
	//for each symbol on the RHS of the rule
	for (int i = 0; i < strlen(c->R); i++) {
		//set status of terminals or nullables to 1
		if (islower(c->R[i]) || c->R[i] == '#') {
			int x = getSymbolIndex(c->R[i]);
			status[x] = 1;
		}
	}

	//if the production contains any non-terminable variables (status = 0) on RHS, return 0
	for (int i = 0; i < strlen(c->R); i++) {
		int x = getSymbolIndex(c->R[i]);
		if (status[x] <= 0)
			return 0;
	}

	//return 1 if all terminals on RHS (or terminable Vars)
	return 1;
}

//utility to print status of symbols
void printStatus()
{
	printf("Status => [");
	for (int i = 0; i < sc; i++)
		printf("%d, ", status[i]);
	printf("]\n");
}

//removes useless symbols
void removeUselessSymbols()
{
	//reset status
	for (int i = 0; i < sc; i++)
		status[i] = 0;

	//repeat a few times to ensure all unreachable symbols are found
	for (int j = 0; j < 3; j++)
		//start from last production rule for convenience
		for (int i = n-1; i >= 0; i--)
			//if it has all terminals, then set status of the producer to 1 as well
			if (isTerminable(&G[i]) == 1)
				status[getSymbolIndex(G[i].L)] = 1;

	//print the symbol statuses
	printf("Non-Terminable Symbols Are Indicated By 0:\n[%s]\n[", symbols);
	for (int i = 0; i < sc; i++)
		printf("%d", status[i]);
	printf("]\n");


	//remove non terminable productions
	for (int i = 0; i < n; i++) {
		//remove the ones that are immediately 0
		if (status[getSymbolIndex(G[i].L)] == 0) {
			G[i].present = 0;
			continue;
		}

		//for the rest, for each symbol in RHS
		for (int j = 0; j < strlen(G[i].R); j++) {
			//if the symbol in RHS is non-terminable, then the entire rule itslef is non-terminable
			if (status[getSymbolIndex(G[i].R[j])] == 0) {
				G[i].present = 0;
				break;
			}
		}
	}

	//it is assumed that the LHS of the first inputed producton is the start symbol 'S'
	status[0] = 2;

	//repeat a few times to make sure
	for (int j = 0; j < 3; j++) {
		//for each production
		for (int i = 0; i < n; i++) {
			//neglect already removed productions
			if(G[i].present == 0)
				continue;

			//for the others, if the producer is reachable, then all the symbols on the RHS are as well
			if (isReachable(G[i].L) == 1)
				for (int k = 0; k < strlen(G[i].R); k++)
					status[getSymbolIndex(G[i].R[k])] = 2;
		}
	}

	//print non-reachables
	printf("Non-Terminable Symbols Are Indicated By 1:\n[%s]\n[", symbols);
	for (int i = 0; i < sc; i++)
		printf("%d", status[i]);
	printf("]\n");


	//remove non reachable productions
	for (int i = 0; i < n; i++) {
		//remove the ones that are immediately 1
		if (status[getSymbolIndex(G[i].L)] == 1) {
			G[i].present = 0;
			continue;
		}

		//for the rest, for each symbol in RHS
		for (int j = 0; j < strlen(G[i].R); j++) {
			//if the symbol in RHS is non-reachable, then the entire rule itself should be removed
			if (status[getSymbolIndex(G[i].R[j])] == 1) {
				G[i].present = 0;
				break;
			}
		}
	}

	//print new grammar
	printf("Updated Grammar Rules after removing Useless Symbols:\n");
	printGrammar();
}

//checks if production with index ind is present already in G
int isProductionPresent(int ind)
{

	for (int i = 0; i < n; i++) {
		if (i == ind)
			continue;
		if (G[i].present == 0)
			continue;
		if (G[i].L == G[ind].L && strcmp(G[i].R,G[ind].R) == 0)
			return 1;
	}
	return 0;
}

//removes the nullable symbols and adds new productions to G
void addNonNull(struct rule *r)
{
	//if null then remove production
	if (r->R[0] == '#')
		r->present = 0;

	//if already removed or contains only one symbol on RHS return
	if (r->present == 0)
		return;
	if (strlen(r->R) <=1)
		return;

	//for each symbol in RHS
	for (int i = 0; i < strlen(r->R); i++) {
		//if the symbol is nullable then we must remove the null
		if (status[getSymbolIndex(r->R[i])] == 1) {
			printf("Removing Null from %c -> %s\n", r->L, r->R);

			//make a new production copy
			G[n].L = r->L;
			sprintf(G[n].R,"%s",r->R);

			//shift the following symbols to the left by 1 to overwrite the current
			int j;
			for (int j = i; j < strlen(r->R); j++)
				G[n].R[j] = G[n].R[j+1];
			//G[n].R[j] = '\0';	-> this was causing an error

			//S->S or A->A can be added so we gotta be careful
			if (strlen(G[n].R) == 1) {
				if (G[n].L == G[n].R[0]) {
					G[n].present = 0;
					continue;
				}
			}
			G[n].present = 1;
			n++;

			//add if the production doesn't already exist
			printf("Adding %c -> %s if not already present\n",G[n-1].L,G[n-1].R);
			if (isProductionPresent(n-1) == 1) {
				n--;
				G[n].present = 0;
				continue;
			}

			//recur with this
			addNonNull(&G[n-1]);
		}
	}
}

//removes Null productions from the grammar
void removeNullProductions()
{
	//reset statuses
	for (int i = 0; i < sc; i++) {
		if (status[i] < 2)
			status[i] = 0;
		else status[i] = 2;

		if (symbols[i] == '#')	//null -> 1
			status[i] = 1;
	}

	//repeat a few times
	for (int j = 0; j < 3; j++) {
		//for every production in reverse order
		for (int i = n-1; i >= 0; i--) {
			//skip already removed productions
			if (G[i].present == 0)
				continue;

			//for the remaining, for each symbol of RHS
			int nullableFlag = 1;
			for (int k = 0; k < strlen(G[i].R); k++) {
				//if any symbol is not null/nullable then the LHS is not nullable
				if (status[getSymbolIndex(G[i].R[k])] != 1) {
					nullableFlag = 0;
					break;
				}
			}
			if (nullableFlag)
				status[getSymbolIndex(G[i].L)] = 1;
		}
	}

	//print nullable symbols
	printf("Nullable Symbols Are Indicated By 1:\n[%s]\n[", symbols);
	for (int i = 0; i < sc; i++)
		printf("%d", status[i]);
	printf("]\n");

	//permute the nullables and add for each production
	for (int i = n-1; i >= 0; i--)
		addNonNull(&G[i]);

	//print new grammar
	printf("Updated Grammar Rules after removing Null Productions:\n");
	printGrammar();
}

//utility for removing unit productions
int removeUnit(struct rule *r)
{
	int limit = n;		//we dont want an inifite loop as we are doing n++
	int found = 0;

	//check all original productions
	for (int i = 0; i < limit; i++) {
		//skip already removed
		if (G[i].present == 0)
			continue;

		//compare with A->a (G[i]) and S->A (r) eg.
		if (G[i].L == r->R[0]) {
			found = 1;	//found

			//add S->a to the end and remove S->A, for example
			G[n].L = r->L;
			sprintf(G[n].R,"%s",G[i].R);
			G[n].present = 1;
			n++;
			r->present = 0;
		}
	}
	return found;
}

//Removes unit productions
void removeUnitProductions()
{
	//repeat few times
	for (int j = 0; j < 3; j++) {
		//productions in reverse order
		for (int i = n-1; i >= 0; i--) {
			//skip already removed
			if (G[i].present == 0)
				continue;

			//if RHS is a single symbol and a variable
			if (strlen(G[i].R) == 1) {
				if (isupper(G[i].R[0])) {
					//try to remove it
					printf("Remove %c -> %s\n", G[i].L, G[i].R);
					if (removeUnit(&G[i]) == 1)
						printf("Removed\n");
				}
			}
		}
	}

	//print new grammar
	printf("\nUpdated Grammar Rules after removing Unit Productions:\n");
	printGrammar();
}

//prints answer for LR
void printAnswer()
{
	//print answer
	printf("Grammar => [\n");
	for (int i = 0; i < nl; i++)
		printf("%s -> %s\n", ans[i].L, ans[i].R);
	printf("]\n\n");
}

//utility that removes single left recursion
void removeSingleLR(struct rule *r)
{
	//new variable
	char Z[10];
	sprintf(Z, "Z%d" , zcounter);

	//loop through all productions
	int count = 0;
	for (int j = 0; j < n; j++) {
		//skip already removed
		if (G[j].present == 0)
			continue;

		//For other productions of the same LHS without left recursion,
		// eg. G[j]: S->ASB & S->AB, r: S->SB
		if (r->L == G[j].L && (G[j].L != G[j].R[0])) {
			count++;

			//further times - so strcat to first productions Right side
			//1) Z->ASBZ1 | ABZ1
			if (count > 1) {
				char str[20];
				sprintf(str, " | %s%s",G[j].R, Z);
				strcat(ans[nl].R, str);
			}
			//first time
			else
			{
				//Put LHS of L in ans, and RHS of G[j] followed by Z
				//1) S->ASBZ1
				sprintf(ans[nl].L, "%c", r->L);
				sprintf(ans[nl].R, "%s%s", G[j].R, Z);

				//Remove first letter of r's RHS to get
				//B
				char str[20];
				sprintf(str, "%s", r->R);
				for (int k = 0; k < strlen(r->R); k++)
					str[k] = str[k+1];

				//update the next one: Z -> (r'sRHS-first)Z | #
				//2) Z1->BZ1 | #
				sprintf(ans[nl+1].R,"%s%s | #",str,Z);
				sprintf(ans[nl+1].L,"Z%d",zcounter);

			}
			printf("1) %s -> %s\n", ans[nl].L, ans[nl].R);
			printf("2) %s -> %s\n", ans[nl+1].L, ans[nl+1].R);
		}
	}

	//increment counters
	nl += 2;
	zcounter++;
}

//function to remove left recursion
void removeLeftRecursion()
{
	//new counter
	nl = 0;

	//for each production
	for (int i = 0; i < n; i++) {
		//skip already removed
		if (G[i].present == 0)
			continue;

		//if S->Sabsfba type: remove recursion
		if (G[i].L == G[i].R[0])
			removeSingleLR(&G[i]);
		else
		{
			//else its correct, add it to answer
			sprintf(ans[nl].L, "%c", G[i].L);
			sprintf(ans[nl].R, "%s", G[i].R);
			nl++;
		}
	}

	//print output
	printf("\nGrammar after removing Left Recursion is:\n");
	printAnswer();
}
