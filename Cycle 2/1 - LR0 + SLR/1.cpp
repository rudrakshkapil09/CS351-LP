#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <iomanip>
#include <queue>
using namespace std;

//structs
struct rule {
    int number;
    char L;
    string R;
    int dot_pos;
};

//actually itemset
struct itemset {
    int number;
    vector<rule> r;
};

//for the SLR table
struct cell {
    char action;    //S, R, or G
    int num;
};

struct SLRTable {
    int ncols;
    int nrows;
    vector<vector<cell> > T;

    SLRTable(int x, int y) {
        nrows = x;
        ncols = y;
        T = vector<vector<cell>>(nrows, vector<cell>(ncols));
    }
};

struct GOTO {
    int from, to;
    char by;
};

struct ACTION {
    int from, to;
    char by;
};


//globals
struct vector<itemset> items;
struct vector<rule> G;
struct SLRTable table(0,0);         //actual table
set<char> allVarsSet, allTerminalsSet;
vector<char> allVars, allTerminals;
vector<GOTO> gotoTable;
vector<ACTION> actionTable;
map<vector<rule>, int> present;    //used to check if itemset that would be produced after closure is already present
                                //i.e. if parameter of getClosure is already present

//for map
bool operator <(const rule &a, const rule &b)
{
    if (a.L != b.L)
        return a.L < b.L;
    else return a.R < b.R;
}

//function to print grammar
void printGrammar(vector<rule> V)
{
    cout << " [ \n";
    for (int i = 0; i < V.size(); i++)
        cout << setw(3) << V[i].L << "->" << V[i].R << "\n";
    cout << " ] \n";
}

//function to get initial grammar
void getGrammar()
{
    string ip;
    cout << "Enter grammar rules with LHS as first letter: (end with 0)\n";

    //add production 0
    struct rule r;
    r.number = 0;
    r.L = 'Y';
    r.R = "E";
    G.push_back(r);

    //get the other productions
    int i = 1;
    while (true) {
        cin >> ip;
        if (ip == "0")
            break;

        struct rule t;
        t.number = i++;
        t.L = ip[0];
        t.R = ip.substr(1);
        G.push_back(t);

        //add relevant characters to all symbols
        allVarsSet.insert(t.L);
        for (int j = 0; j < t.R.size(); j++)
            if (!(t.R[j] >= 'A' && t.R[j] <= 'Z'))
                allTerminalsSet.insert(t.R[j]);
    }

    cout << "\nGrammar =>\n";
    printGrammar(G);
}

//function that returns closure of given Variable only
vector<rule> getClosure(char P)
{
    vector<rule> closure;    //to be returned

    //for each production, if LHS is matching, push in @ followed by RHS
    for (int i = 0; i < G.size(); i++)
        if (G[i].L == P) {
            struct rule r;
            r.L = P;
            r.R = "."+G[i].R;
            r.dot_pos = 1;
            closure.push_back(r);
        }

    return closure;
}


//gives closure for all symbols
vector<rule> getFullClosure(vector<rule> V)
{
    vector<rule> ans;
    for (int i = 0; i < V.size(); i++)
        ans.push_back(V[i]);

    for (int i = 0; i < V.size(); i++) {
        int pos = V[i].dot_pos;
        char s = V[i].R[pos];    //current dotted symbol for this itemset

        //if its a production, get closure
        int found = 0;
        vector<rule> closure;
        if (s >= 'A' && s <= 'Z') {
            closure = getClosure(s);
            found = 1;
        }

        //add original and if needed then the closure
        if (found)
            for (int j = 0; j < closure.size(); j++)
                ans.push_back(closure[j]);
    }

    return ans;
}

//utilities for printing itemsets
void printItem(int i)
{
    cout << "\nI" << items[i].number << ":\n";
    printGrammar(items[i].r);
}
void printItems()
{
    for (int i = 0; i < items.size(); i++)
        printItem(i);
}

int itemset_count = 1;
void getItemSets(vector<rule> curr)
{
    //for each production, store the previous state number and the character which brought it here
    queue<pair<vector<rule>,pair<int,char>> > Q;
    Q.push(make_pair(curr,make_pair(0,'-')));
    int from;
    char by;

    while (Q.size() > 0) {
        curr = Q.front().first;
        from = Q.front().second.first;
        by = Q.front().second.second;
        Q.pop();

        //find all symbols that can be shifted
        set<char> symbols;
        for (int i = 0; i < curr.size(); i++)
            if (curr[i].dot_pos <= curr[i].R.size())
                symbols.insert(curr[i].R[curr[i].dot_pos]);

        //for each symbol,
        set<char> :: iterator it;
        for (it = symbols.begin(); it != symbols.end(); it++) {
            char c = *it;

            //find each production that has that symbol at the dot position
            vector<rule> newset;
            for (int i = 0; i < curr.size(); i++) {
                int pos = curr[i].dot_pos;
                char s = curr[i].R[pos];    //current dotted symbol for this itemset

                //matching - increase dot position and push into newset
                if (c == s && pos < curr[i].R.size()) {
                    struct rule newRule = {
                        .L = curr[i].L,
                        .R = curr[i].R
                    };
                    swap(newRule.R[pos], newRule.R[pos-1]);
                    newRule.dot_pos = pos+1;
                    newset.push_back(newRule);
                }
            }

            //only insert if it's a new set -> closure will also be unique
            if (newset.size() > 0 && present.find(newset) == present.end()) {
                present[newset] = itemset_count;

                //find closure and get itemset
                struct itemset temp = {
                    .number = itemset_count,
                    .r = getFullClosure(newset)
                };
                items.push_back(temp);

                //recur later
                pair<vector<rule>, pair<int,char> > to_push;
                to_push = make_pair(temp.r, make_pair(itemset_count, c));
                Q.push(to_push);

                //put into GOTO table
                if (c >= 'A' && c <= 'Z') {
                    struct GOTO g = {.from = from, .to = itemset_count, .by = c};
                    gotoTable.push_back(g);
                }
                else {
                    struct ACTION a = {.from = from, .to = itemset_count, .by = c};
                    actionTable.push_back(a);
                }

                //increment
                itemset_count++;
            }
            else {
                //put into GOTO table
                if (c >= 'A' && c <= 'Z') {
                    struct GOTO g = {.from = from, .to = present[newset], .by = c};
                    gotoTable.push_back(g);
                }
                else {
                    struct ACTION a = {.from = from, .to = present[newset], .by = c};
                    actionTable.push_back(a);
                }
            }
        }
    }
}

//function to print SLR table
void printTable()
{
    //print headers - first terminals then variables
    cout << "\n\n[PARSING TABLE]\n   SET   | ";
    set<char> :: iterator it;
    for (int i = 0; i < allTerminals.size(); i++)
        cout << setw(5) << allTerminals[i] << "     ";
    cout << "|";
    for (int i = 0; i < allVars.size(); i++)
        cout << setw(5) << allVars[i] << "     ";
    cout << "\n---------+";
    for (int i = 0; i < allTerminals.size(); i++)
        cout << "----------";
    cout << "-+";
    for (int i = 0; i < allVars.size(); i++)
        cout << "----------";
    printf("\n");

    //print each row
    for (int i = 0; i < table.nrows; i++) {
        printf("%5d    |", items[i].number);

        //print the action cols
        for (int j = 0; j < allTerminals.size(); j++) {
            char res[3];
            res[0] = table.T[i][j].action;
            sprintf(res+1,"%d", table.T[i][j].num);
            printf("%6s    ", res);
        }

        //print the goto cols
        cout << " |";
        for (int j = 0; j < allVars.size(); j++){
            char res[3];
            res[0] = table.T[i][j+allTerminals.size()].action;
            sprintf(res+1,"%d", table.T[i][j+allTerminals.size()].num);
            printf("%6s    ", res);
        }


        cout << "\n";
    }
}

//returns index of c in V
int getIndex(vector<char> V, char c)
{
    for (int i = 0; i < V.size(); i++)
        if (c == V[i])
            return i;
    return -1;
}

void populateTable()
{

    //for each in action - shift
    for (int i = 0; i < actionTable.size(); i++) {
        int from = actionTable[i].from;
        int to = actionTable[i].to;
        int by_ind = getIndex(allTerminals, actionTable[i].by);

        if (by_ind >= 0)
            table.T[from][by_ind] = {.action = 'S', .num = to};
    }

    //for each in goto - goto
    for (int i = 0; i < gotoTable.size(); i++) {
        int from = gotoTable[i].from;
        int to = gotoTable[i].to;
        int by_ind = getIndex(allVars, gotoTable[i].by);

        table.T[from][by_ind+allTerminals.size()] = {.action = 'G', .num = to};
    }

    //now, reduce
    //find first and follow of E
    set<char> first;
    set<char> follow;
    follow.insert('$'); //since starting prod

    //go through each production and
    for (int i = 0; i < G.size(); i++) {
        //if RHS[0] is terminal, put it in first
        if (G[i].L == 'E' && (G[i].R[0] < 'A' || G[i].R[0] > 'Z'))
            first.insert(G[i].R[0]);

        //go through RHS and if prev is E, add to follow
        for (int j = 1; j < G[i].R.size(); j++)
            if (G[i].R[j-1] == 'E' && ((G[i].R[j] < 'A' || G[i].R[j] > 'Z')))
                follow.insert(G[i].R[j]);
    }

    //reduce part:

}

//MAIN
int main()
{
    //get grammar and symbol vectors
    allVars.clear();
    allTerminals.clear();
    getGrammar();
    allVars = vector<char>(allVarsSet.begin(), allVarsSet.end());
    allTerminals = vector<char>(allTerminalsSet.begin(), allTerminalsSet.end());
    allTerminals.push_back('$');

    //Y->@E
    vector<rule> tempv;
    struct rule r;
    r.L = 'Y';
    r.R = ".E";
    r.dot_pos = 1;
    tempv.push_back(r);

    struct itemset itemZero;
    itemZero.number = 0;
    itemZero.r = getFullClosure(tempv);
    items.push_back(itemZero);

    getItemSets(itemZero.r);
    printItems();

    //create table
    int ncols = (int) (allVars.size() + allTerminals.size());
    table = SLRTable(itemset_count, ncols);

    //populate table:
    populateTable();
    printTable();
}

/*
EE+E
EE*E
E(E)
Ea
0

*/
