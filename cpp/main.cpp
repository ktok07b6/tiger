#include <string.h>
#include <errno.h>

#include "Absyn.h"
#include "parser.hpp"
#include "AbsynPrinter.h"
#include "AST2DotTranslater.h"
#include "TypeCheck.h"
#include "FindEscape.h"
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
#include "Instruction.h"

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

bool parsePhase()
{
#ifdef YYDEBUG
	yydebug = 1;
#endif
	yyparse();
	absyn = result_syntax_tree;
	if (!absyn) {
		return false;
	}
	printAbsyn();
	return true;
}

void addPreInstallFuncs()
{
	Symbol *print = Symbol::symbol("print");
	TypeList print_args;
	print_args.push_back(StrT);
	FuncEntry *fe = new FuncEntry(VoidT, print_args);
	nameTable.put(print, fe);
}

bool typeCheckPhase()
{
	printf("Finding escape value===========\n\n");
	FindEscape *findEscape = new FindEscape();
	absyn->accept(findEscape);
	delete findEscape;

	printf("Type checkn===========\n\n");
	//put default symbols
	nameTable.beginScope(0);
	typeTable.beginScope(0);

	addPreInstallFuncs();

	Symbol *ints = Symbol::symbol("int");
	Symbol *strs = Symbol::symbol("string");
	typeTable.put(ints, new class IntT());
	typeTable.put(strs, new class StrT());

	//-----------------------
	bool ok = typeCheck();
	//printSource();

	nameTable.endScope();
	typeTable.endScope();
	return ok;
}

void printSymbols()
{
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
}

void translatePhase(FragmentList &fragments)
{
	std::vector<int> formals;
	Frame *frame = Frame::newFrame(NULL, formals);
	IRTranslater translater(frame);
	absyn->accept(&translater);
	fragments = translater.getFragments();
#if 1
	//ir = translater.getExp();
	printf("Intermediate Representation===========\n\n");
	//tree::TreePrinter::printTree(ir->unNx());
	
	FragmentList::iterator it;
	it = fragments.begin();
	while (it != fragments.end()) {
		Fragment *frag = *it;
		DBG("%s", frag->toString().c_str());
		++it;
	}
	
	printf("======================================\n\n");
#endif
}

void codegenPhase2(assem::InstructionList *instList, TempMap *tempMap);

void codegenPhase(const FragmentList &frags)
{
	printSource();

	FragmentList::const_iterator it;
	it = frags.begin();
	while (it != frags.end()) {
		Fragment *frag = *it;

		if (frag->isProc()) {
			ProcFragment *proc = (ProcFragment*)frag; 
			Canon canon;
			tree::StmList stms;
			stms = canon.linearize(proc->getStm());
#if 1
			printf("Linearized IR=========================\n\n");
			tree::TreePrinter::printStmList(stms);
			printf("======================================\n\n");
		
#endif
			
			BasicBlocks bb(stms);
#if 0
			printf("Basic Blocks=========================\n\n");
			std::list< tree::StmList >::const_iterator i1 = bb.blocks.begin();
			while (i1 != bb.blocks.end()) {
				DBG("-----");
				const tree::StmList &block = (*i1);
				tree::TreePrinter::printStmList(block);
				++i1;
				
			}
			printf("======================================\n\n");
#endif

			Trace trace(bb.blocks);
#if 0
			printf("Traces=========================\n\n");
			tree::TreePrinter::printStmList(trace.traced);
			printf("======================================\n\n");
#endif

			printf("CodeGen=========================\n\n");
			Frame *frame = proc->getFrame();
			stms = trace.traced;
			tree::StmList::iterator it = stms.begin();
			while (it != stms.end()) {
				tree::Stm *stm = *it;
				assem::InstructionList *instList = frame->codegen(stm);
				codegenPhase2(instList, frame);
				++it;
			}
		} else {
			assert(frag->isData());
			DataFragment *data = (DataFragment*)frag;
			DBG("%d", data->toString().c_str());
		}
		++it;
	}
}

void codegenPhase2(assem::InstructionList *instList, TempMap *tempMap)
{
	assem::InstructionList::iterator it;
	it = instList->begin();
	while (it != instList->end()) {
		assem::Instruction *inst = *it;
		std::string s = inst->format(tempMap);
		DBG("%s", s.c_str());
		++it;
	}
}

int main(int argc, char **argv)
{
	yyin = NULL;
	if (argc==2) {
		yyin = fopen(argv[1], "rb");
		if (yyin == NULL) {
			fprintf(stderr, "tiger: %s (%s)\n", strerror(errno), argv[1]);
			exit(-1);
		}
	} else {
		yyin = stdin;
	}
	initialize();

	if (!parsePhase()) {
		return -1;
	}
	//makeDot();
	//printSource();
	if (!typeCheckPhase()) {
		return -1;
	}

	//printSymbols();

	FragmentList frags;
	translatePhase(frags);

	codegenPhase(frags);

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
