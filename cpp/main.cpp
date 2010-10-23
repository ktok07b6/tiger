#include <string.h>
#include "Absyn.h"
#include "parser.hpp"
#include "AbsynPrinter.h"
#include "AST2DotTranslater.h"
#include "TypeCheck.h"
#include "debug.h"
#include "Type.h"
#include "NameEntry.h"
#include "Table.h"
#include "tiger.h"
#include "IRTranslater.h"
#include "Frame.h"
#include "TreePrinter.h"
#include "Exp.h"
#include "HeapManager.h"
#include "Canon.h"
#include "BasicBlocks.h"
#include "Trace.h"

extern int yylex();
extern int yyparse();
extern int yydebug;

Absyn *result_syntax_tree;
Scopes<NameEntry> nameScopes;
Scopes<Type> typeScopes;
Table<NameEntry> nameTable(nameScopes);
Table<Type> typeTable(typeScopes);

extern FILE *yyin;
Absyn *absyn = NULL;
translate::Exp *ir = NULL;

void printAbsyn();
void makeDot();
bool typeCheck();
void printSource();

int main(int argc, char **argv)
{
#ifdef YYDEBUG
	yydebug = 1;
#endif
	if (argc==2) {
		yyin = fopen(argv[1], "rb");
	} else {
		yyin = stdin;
	}
	
	initialize();
	yyparse();
	absyn = result_syntax_tree;
	if (!absyn) {
		return -1;
	}
	printAbsyn();
	//makeDot();
	//printSource();
    //return 0;
	//-----------------------
	//put default symbols
	nameTable.beginScope(0);
	typeTable.beginScope(0);

	Symbol *print = Symbol::symbol("print");
	TypeList print_args;
	print_args.push_back(StrT);
	FuncEntry *fe = new FuncEntry(VoidT, print_args);
	nameTable.put(print, fe);

	Symbol *ints = Symbol::symbol("int");
	Symbol *strs = Symbol::symbol("string");
	typeTable.put(ints, new class IntT());
	typeTable.put(strs, new class StrT());

	//-----------------------
	bool ok = typeCheck();
	printSource();

	if (!ok) {
		return -1;
	}
	nameTable.endScope();
	typeTable.endScope();
#if 1
	Scopes<NameEntry>::ScopeMap::iterator it = nameScopes.dict.begin();
	while (it != nameScopes.dict.end()){
		Scopes<NameEntry>::ScopeMapEntry entry = (*it);
		unsigned int id = entry.first;
		Scopes<NameEntry>::List li = entry.second;
		Scopes<NameEntry>::List::iterator i2 = li.begin();
		while (i2 != li.end()) {
			BindInScope<NameEntry> bind = (*i2);
			Symbol *name = bind.key;
			NameEntry *value = bind.value;
			DBG("name = %s, value = %s", name->name.c_str(), value->toString().c_str());
			++i2;
		}
		++it;
	}
#endif

#if 1
	{
		std::vector<int> formals;
		Frame *frame = Frame::newFrame(NULL, formals);
		IRTranslater translate(frame);
		absyn->accept(&translate);
		ir = translate.texp;
		printf("Intermediate Representation===========\n\n");
		tree::TreePrinter::printTree(ir->unNx());
		printf("======================================\n\n");
	}
#endif

	{
		Canon canon;
		tree::StmList stms;
		stms = canon.linearize(ir->unNx());
		printf("Linearized IR=========================\n\n");
		tree::TreePrinter::printStmList(stms);
		printf("======================================\n\n");

#if 1
		printf("Basic Blocks=========================\n\n");
		BasicBlocks bb(stms);
		std::list< tree::StmList >::const_iterator i1 = bb.blocks.begin();
		while (i1 != bb.blocks.end()) {
			DBG("-----");
			const tree::StmList &block = (*i1);
			tree::TreePrinter::printStmList(block);
			++i1;
		}
#endif

#if 1
		{
			printf("Traces=========================\n\n");
			Trace trace(bb.blocks);
			tree::TreePrinter::printStmList(trace.traced);
		}
#endif

	}
	//HeapManager::instance()->dump();
	HeapManager::instance()->clean();
	return 0;
}

void printAbsyn()
{
	AbsynPrinter p;
	absyn->accept(&p);
	std::string result = p.pop();
	DBG("%s", result.c_str());
}


void makeDot()
{
	AST2DotTranslater dot;
	absyn->accept(&dot);
	std::string result = dot.pop();
	result = "\ndigraph tiger{\n" + result + "}\n";
	DBG("%s", result.c_str());
	FILE *dotf = fopen("test.dot", "w");
	fwrite(result.c_str(), result.size(), 1, dotf);
	fclose(dotf);
}

bool typeCheck()
{
	TypeCheck checker;

	try {
		absyn->accept(&checker);
	} catch (const std::string &msg) {
		ERROR("%s", msg.c_str());
	}

	DBG("type checked %d", checker.isAccepted());
	return checker.isAccepted();
}

void printSource()
{
	fseek(yyin, 0, SEEK_SET);
	int buf_size = 256;
	char buf[256];
	while(fgets(buf, buf_size-1, yyin) != NULL) {
		
		//buf[n] = '\0';
		//	printf("%s", buf);
		buf[strlen(buf)-1] = '\0';
		puts(buf);
		/*if (n < 32) {
			printf("\n");
			break;
			}*/
	}
}
