%option noyywrap

%{
    //include the header
    #include "DC.tab.hpp"
%}

/* float exponent */
EXP	([Ee][-+]?[0-9]+)

%%
[0-9]+"."[0-9]*{EXP}? |
"."?[0-9]+{EXP}? 		{ yylval.n = atof(yytext); return NUM; }

"IF"                    { return IF;}
"ELSE"                  { return ELSE;}
"THEN"                  { return THEN;}
"DISPLAY"               { return DISPLAY;}
"LET"                   { return LET;}

[a-zA-Z][a-zA-Z0-9]*	{ strcpy(yylval.Str, yytext); return ID; }

"=="                    { return EE;}
"<="                    { return LE;}
">="                    { return GE;}
"!="                    { return NE;}

[<>()+-@*/]             { return yytext[0];}
[\t' ']+
[\n]                    { return ENDL;}
.
%%
