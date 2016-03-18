#include "hpp.hpp"
#define YYERR "\n\n"<<yylineno<<":"<<msg<<"["<<yytext<<"]\n\n"
void yyerror(string msg) { cout<<YYERR; cerr<<YYERR; exit(-1); }
int main() { env_init(); return yyparse(); }

Sym::Sym(string T,string V) { tag=T; val=V; }
Sym::Sym(string V):Sym("",V) {}

void Sym::push(Sym*o) { nest.push_back(o); }
void Sym::par(Sym*o) { pars[o->val]=o; }

string Sym::tagval() { ostringstream os; os<<"<"<<tag<<":"<<val<<":"<<this<<">";
	return os.str(); }
string Sym::tagstr() { ostringstream os; os<<"<"<<tag<<":'"<<val<<"':"<<this<<">";
	return os.str(); }
string Sym::pad(int n) { string S; for (int i=0;i<n;i++) S+='\t'; return S; }
string Sym::dump(int depth) { string S = "\n"+pad(depth)+tagval();
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		S += "\n" + pad(depth+1) + pr->first + pr->second->dump(depth+2);
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		S += (*it)->dump(depth+1);
	return S; }

Sym* Sym::eval() {
	Sym*E = env[val]; if (E) return E;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it) = (*it)->eval();
	return this; }

Sym* Sym::eq(Sym*o) { env[val]=o; return o; }
Sym* Sym::at(Sym*o) { push(o); return this; }
Sym* Sym::pfxadd() { val = "+"+val; return this; }
Sym* Sym::pfxsub() { val = "-"+val; return this; }

Str::Str(string V):Sym("str",V) {}
string Str::tagval() { return tagstr(); }

Int::Int(string V):Sym("int","") { val = atol(V.c_str()); }
string Int::tagval() { ostringstream os; os<<"<"<<tag<<":"<<val<<":"<<this<<">";
	return os.str(); }
Sym* Int::pfxadd() { val = +val; return this; }
Sym* Int::pfxsub() { val = -val; return this; }

Num::Num(string V):Sym("num","") { val = atof(V.c_str()); }
string Num::tagval() { ostringstream os; os<<"<"<<tag<<":"<<val<<":"<<this<<">";
	return os.str(); }
Sym* Num::pfxadd() { val = +val; return this; }
Sym* Num::pfxsub() { val = -val; return this; }

Hex::Hex(string V):Sym("hex",V) {}
Bin::Bin(string V):Sym("bin",V) {}

List::List():Sym("","") {}

Op::Op(string V):Sym("op",V) {}

Sym* Op::eval() { Sym::eval();
	if (val=="=") return nest[0]->eq(nest[1]);
	if (val=="@") return nest[0]->at(nest[1]);
	return this;
}

Lambda::Lambda():Sym("^","^") {}

Sym* Sym::copy() {
	Sym* R = new Sym(tag,val);
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		R->pars[pr->first] = pr->second;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		R->push(*it);
	return R;
}

Sym* Sym::replace(string S,Sym*o) {
	if (val==S) return o;
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		if (pr->first==S) pr->second=o;
	for (auto it=nest.begin(),e=nest.end();it!=e;it++)
		(*it)=(*it)->replace(S,o);
	return this;
}

Sym* Lambda::at(Sym*o) {
	Sym* R = copy();
	for (auto pr=pars.begin(),e=pars.end();pr!=e;pr++)
		R=R->replace(pr->first,o);
	return R->nest[0];
}

map<string,Sym*> env;
void env_init(){}
