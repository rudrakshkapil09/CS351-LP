#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAXPRODS 10

//globals
char startVar, string[30];
int numProd, size;
int tracker[26] = {0};			//to track how many of each character
char prods[MAXPRODS][30];
char generates[26][10][30];
char matrix[30][30][30];

//gets initial input and stores
void getInformation()
{
	printf("Enter Starting Variable: ");
	scanf("%c", &startVar);

	printf("Enter Number Of Productions: ");
	scanf("%d", &numProd);
	if (numProd >= MAXPRODS) {
		printf("Exceeded Max Productions!\n");
		abort();
	}

	printf("Enter Productions:\n");
	for (int i = 0; i < numProd; i++)
		scanf("%s", prods[i]);

	printf("Enter string to check: \n");
	scanf("%s", string);
	for (;string[size] != '\0'; size++);
}

//prints error if noti n CNF and aborts
void printCNFError(int i, int j)
{
	printf("Production %d is not in CNF!\nCODE: %d\n", i, j);
	abort();
}

void addToGenerates(int end, int start, int i)
{
	int len = end-start, m = 0;
	int starter = prods[i][0] - 'A';

	for (int k = start; k < end; k++, m++)
		generates[starter][tracker[starter]][m] = prods[i][k];

	generates[starter][tracker[starter]][m] = '\0';
	tracker[starter]++;
}


//checks if all productions are in CNF
void checkCNF()
{
	//for each production
	for (int i = 0; i < numProd; i++) {
		//check start
		if (prods[i][2] != '>')
			printCNFError(i, 0);

		//check each boundary
		int j = 3, start = 3, end, lowercase = 0;
		while (prods[i][j] != '\0') {
			//check boundary sizes
			if (prods[i][j] == '|') {
				end = j;

				if (end - start > 2 || end - start < 1)
					printCNFError(i, 1);
				else if (end - start == 2 && lowercase)
					printCNFError(i, 2);
				else if (end - start == 1 && !lowercase)
					printCNFError(i, 3);

				//at this point we can extract the part of the production and store in generates
				addToGenerates(end, start, i);

				//continue for next
				start = end + 1;
			}
			//check if within boundaries the letters are correct
			else {
				if (prods[i][start] >= 'a' && prods[i][start] <= 'z')
					lowercase = 1;
				else if (prods[i][start] >= 'A' && prods[i][start] <= 'Z')
					lowercase = 0;
				else printCNFError(i, 4);

				if (lowercase && prods[i][start] >= 'A' && prods[i][start] <= 'Z')
					printCNFError(i, 5);
				if (!lowercase && prods[i][start] >= 'a' && prods[i][start] <= 'z')
					printCNFError(i, 6);
			}
			j++;
		}

		//add the last generation
		addToGenerates(j, start, i);
	}
}

//utility for checking
void printGeneratesTable()
{

	for (int i = 0; i < 26; i++) {
		if (tracker[i] == 0)
			continue;
		printf("\n[%c]\n", (i+'A'));
		for (int j = 0; j < tracker[i]; j++)
			printf("%s\n", generates[i][j]);
	}
}

//returns 1 if c is already there in arr
int alreadyPresent(char arr[], int n, char c)
{
	for (int i = 0; i < n; i++)
		if (arr[i] == c)
			return 1;
	return 0;
}


//returns string containing all variables that produce the given string
void getProducers(char *buff, char producers[])
{
	int k = 0;
	for (; producers[k] != '\0'; k++);

	//check each variable for its generations
	for (int i = 0; i < 26; i++) {
		if (tracker[i] == 0)
			continue;
		for (int j = 0; j < tracker[i]; j++) {
			if (strcmp(generates[i][j], buff) == 0 && alreadyPresent(producers, k, i+'A') == 0) {	//match
				producers[k++] = i + 'A';
				break;
			}
		}
	}
}

//prints Matrix
void printMatrix()
{
	printf("\n------------TABLE----------\n");
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++)
			printf("%5s", matrix[i][j]);
		printf("\n");
	}
}

//fills in first row
void fillInFirstRow()
{
	for (int j = 0; j < size; j++) {
		char temp[1];
		temp[0] = string[j];
		getProducers(temp, matrix[0][j]);
	}
}

//fill in remaining rows
void fillInRemainingRows() {
	//traverse as necessary
	for (int i = 1; i < size; i++) {
		for (int j = 0; j < size-i; j++) {
			//printf("\n[%d][%d]\n", i, j);
			for (int p = 0; p < i; p++) {
				int q = i-p-1;
				int r = j+p+1;

				//printf("[%d][%d] . [%d][%d]\n", p, j, q, r);
				//printf("%s - %s\n", matrix[p][j], matrix[q][r]);

				//get concat:

				for (int k = 0; matrix[p][j][k] != '\0'; k++) {
					for (int l = 0; matrix[q][r][l] != '\0'; l++) {
						char temp[2] = "--";
						temp[0] = matrix[p][j][k];
						temp[1] = matrix[q][r][l];
						if (temp[0] <'A' || temp[0] > 'Z')
							continue;

						//printf("%s ", temp);
						//add producers of this to the current cell i, j
						getProducers(temp, matrix[i][j]);
					}
				}

			}
		}
	}
}

void checkAnswer()
{
	int correct = 0;
	for (int i = 0; matrix[size-1][0][i] != '\0'; i++)
		if (matrix[size-1][0][i] == startVar)
			correct = 1;

	if (correct)
		printf("\n========String can be generated!=========\n");
	else printf("\nxxxxxxxx Cannot be generated xxxxxxxxxxxx\n");
}

//MAIN
int main()
{
	getInformation();
	checkCNF();
	fillInFirstRow();
	fillInRemainingRows();
	printMatrix();
	checkAnswer();
}
