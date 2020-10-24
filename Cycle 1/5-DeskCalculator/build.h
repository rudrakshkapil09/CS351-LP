//Headers & function definitions
#include <iostream>
#include <map>
#include <cmath>
using namespace std;

//tree node struct
typedef
struct node
{
    char type;
    float value;
    char Text[20];
    node * left;
    node * right;
    node * expr;
} *TPTR;

/* build tree functions - */
//E -> ID
//stmt -> DISPLAY ID
TPTR build(char type, char Str[])
{
    TPTR T = new node;
    T->type = type;
    strcpy(T->Text,Str);
    T->left = NULL;
    T->right = NULL;
    T->expr = NULL;
    return T;
}

//E -> -E
TPTR build(char type, TPTR R)
{
    TPTR T = new node;
    T->type = type;
    T->left = R;
    T->right = NULL;
    T->expr = NULL;
    return T;
}

//E -> NUM
TPTR build(char type, float num)
{
    TPTR T = new node;
    T->type = type;
    T->value = num;
    T->left = NULL;
    T->right = NULL;
    T->expr = NULL;
    return T;
}

//stmt -> IF E THEN stmt
//E -> E op E
TPTR build(char type, TPTR L, TPTR R)
{
    TPTR T = new node;
    T->type = type;
    if(T->type=='i')    //for if statement
    {
        T->expr=L;
        T->left=R;
        T->right=NULL;
    }
    else
    {
        T->left=L;
        T->right=R;
        T->expr=NULL;
    }
    return T;
}

//stmt -> LET ID = E
TPTR build(char type, char Str[], TPTR L)
{
    TPTR T = new node;
    T->type = type;
    strcpy(T->Text,Str);
    T->left = L;
    T->right = NULL;
    T->expr = NULL;
    return T;
}

//stmt -> IF E THEN stmt ELSE stmt
TPTR build(char type, TPTR E, TPTR L, TPTR R)
{
    TPTR T = new node;
    T->type = type;
    T->expr = E;
    T->left = L;
    T->right = R;
    return T;
}

//calculate function
float Calculate(TPTR T, map<string, float> &M)
{
    // If then Else Statement
    if (T->type == 'I') {
        if (Calculate(T->expr,M))
            return Calculate(T->left,M);
        else return Calculate(T->right,M);
    }

    // If then statement
    else if (T->type == 'i') {
        if (Calculate(T->expr,M))
            return Calculate(T->left,M);
        else return -1;
    }

    // Displaying a variable
    else if (T->type == 'D')
        cout << M[T->Text] << "\n";

    // Assigning a variable
    else if (T->type == 'L')
        M[T->Text] = Calculate(T->left,M);

    // Number
    else if (T->type == 'N')
        return T->value;

    // Variable
    else if (T->type == 'V')
        return M[T->Text];

    // Unary Minus
    else if (T->type == 'U')
        return -1*Calculate(T->left,M);

    // Other operations
    else if (T->type == '*')
        return Calculate(T->left,M) * Calculate(T->right,M);
    else if (T->type == '+')
        return Calculate(T->left,M) + Calculate(T->right,M);
    else if (T->type == '-')
        return Calculate(T->left,M) - Calculate(T->right,M);
    else if (T->type == '/')
        return Calculate(T->left,M) / Calculate(T->right,M);
    else if (T->type == '<')
        return Calculate(T->left,M) < Calculate(T->right,M);
    else if (T->type == '>')
        return Calculate(T->left,M) > Calculate(T->right,M);
    else if (T->type == 'G')     // Greater than or equal to
        return Calculate(T->left,M) >= Calculate(T->right,M);
    else if (T->type == 'S')        // Less than or equal to
        return Calculate(T->left,M) <= Calculate(T->right,M);
    else if (T->type == 'P')        // Not equal to
        return Calculate(T->left,M) != Calculate(T->right,M);
    else if (T->type == 'Q')       // Equal to
        return Calculate(T->left,M) == Calculate(T->right,M);
    else if (T->type == '@')     // Power operator
        return pow(Calculate(T->left,M),Calculate(T->right,M));
    return -1;
}
