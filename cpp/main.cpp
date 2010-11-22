#include <string.h>
#include <errno.h>
#include <iterator>
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
#include "AsmFlowGraph.h"
#include "Liveness.h"
#include "InterferenceGraph.h"
#include "Color.h"

#include "Property.h"
Property<int> value;

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
	DBG("Finding escape value===========\n\n");
	FindEscape *findEscape = new FindEscape();
	absyn->accept(findEscape);
	delete findEscape;

	DBG("Type checkn===========\n\n");
	//put default symbols
	nameTable.beginScope(0);
	typeTable.beginScope(0);

	addPreInstallFuncs();

	Symbol *ints = Symbol::symbol("int");
	Symbol *strs = Symbol::symbol("string");
	typeTable.put(ints, new struct IntT());
	typeTable.put(strs, new struct StrT());

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
#if 0
	//ir = translater.getExp();
	DBG("Intermediate Representation===========\n\n");
	//tree::TreePrinter::printTree(ir->unNx());
	
	FragmentList::iterator it;
	it = fragments.begin();
	while (it != fragments.end()) {
		Fragment *frag = *it;
		DBG("%s", frag->toString().c_str());
		++it;
	}
	
	DBG("======================================");
#endif
}

void codegenPhase2(assem::InstructionList &instList, Frame *frame);

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
#if 0
			DBG("Linearized IR=========================");
			tree::TreePrinter::printStmList(stms);
			DBG("======================================");
		
#endif
			Frame *frame = proc->getFrame();
			BasicBlocks bb(stms, frame->getEndLabel());
#if 0
			DBG("Basic Blocks=========================");
			std::list< tree::StmList >::const_iterator i1 = bb.blocks.begin();
			while (i1 != bb.blocks.end()) {
				DBG("-----");
				const tree::StmList &block = (*i1);
				tree::TreePrinter::printStmList(block);
				++i1;
				
			}
			DBG("======================================");
#endif

			Trace trace(bb.blocks);
#if 0
			DBG("Traces=========================");
			tree::TreePrinter::printStmList(trace.traced);
			DBG("======================================");
#endif

			DBG("CodeGen=========================");
			
			stms = trace.traced;
			assem::InstructionList instList;
			tree::StmList::iterator it = stms.begin();
			while (it != stms.end()) {
				tree::Stm *stm = *it;
				assem::InstructionList *ilist = frame->codegen(stm);
				std::copy(ilist->begin(), ilist->end(), std::back_inserter(instList));
				++it;
			}
			codegenPhase2(instList, frame);
		} else {
			assert(frag->isData());
			DataFragment *data = (DataFragment*)frag;
			DBG("%d", data->toString().c_str());
		}
		++it;
	}
}

void codegenPhase2(assem::InstructionList &instList, Frame *frame)
{
	assem::InstructionList proc;
	proc = frame->procEntryExit2(instList);

	assem::InstructionList::iterator it;
#if 0
	it = proc.begin();
	while (it != proc.end()) {
		assem::Instruction *inst = *it;
		std::string s = inst->format(frame);
		DBG("%s", s.c_str());
		++it;
	}
#endif
	const graph::AsmFlowGraph flow(proc);
	const regalloc::Liveness liveness(flow);
	const graph::InterferenceGraph *igraph = liveness.getInterferenceGraph();
	//igraph->show();

	TempList regs = frame->registers().all;
	
	regalloc::Color color(*igraph, regs);
	color.coloring();

	assem::InstructionList proc2;
	proc2 = frame->procEntryExit3(proc);
	
	it = proc2.begin();
	while (it != proc2.end()) {
		assem::Instruction *inst = *it;
		std::string s = inst->format(&color);
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

	absyn->accept(&checker);

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
