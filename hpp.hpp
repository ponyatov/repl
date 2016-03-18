#ifndef _H_REPL
#define _H_REPL

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

struct Sym {
	string tag,val;
	Sym(string,string); Sym(string);
	vector<Sym*> nest; void push(Sym*);
	map<string,Sym*> pars; void par(Sym*);
	virtual string dump(int=0);
	virtual string tagval(); string tagstr(); string pad(int);
	virtual Sym* eval();
	virtual Sym* eq(Sym*);
	virtual Sym* at(Sym*);
	virtual Sym* pfxadd();
	virtual Sym* pfxsub();
	Sym* copy();
	Sym* replace(string,Sym*);
};

struct Str: Sym { Str(string); string tagval(); };

struct Int: Sym { Int(string); long val; string tagval();
	Sym*pfxadd(); Sym*pfxsub(); };
struct Num: Sym { Num(string); double val; string tagval();
	Sym*pfxadd(); Sym*pfxsub(); };
struct Hex: Sym { Hex(string); };
struct Bin: Sym { Bin(string); };

struct List: Sym { List(); };

struct Op: Sym { Op(string); Sym*eval(); };

struct Lambda: Sym { Lambda(); Sym*at(Sym*); };

extern map<string,Sym*> env;
extern void env_init();

extern int yylex();
extern int yylineno;
extern char* yytext;
#define TOC(C,X) { yylval.o = new C(yytext); return X; }
extern int yyparse();
extern void yyerror(string);
#include "ypp.tab.hpp"

#endif // _H_REPL
