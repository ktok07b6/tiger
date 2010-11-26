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
	TypeList arg_types;
	//initArray
	arg_types.push_back(IntT);
	arg_types.push_back(IntT);
	nameTable.put(Symbol::symbol("initArray"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//alloc
	arg_types.push_back(IntT);
	nameTable.put(Symbol::symbol("alloc"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//print
	arg_types.push_back(StrT);
	nameTable.put(Symbol::symbol("print"), gcnew(FuncEntry, (VoidT, arg_types)));
	arg_types.clear();

	//flush
	nameTable.put(Symbol::symbol("flush"), gcnew(FuncEntry, (VoidT, arg_types)));
	arg_types.clear();

	//ord
	arg_types.push_back(StrT);
	nameTable.put(Symbol::symbol("ord"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//chr
	arg_types.push_back(IntT);
	nameTable.put(Symbol::symbol("chr"), gcnew(FuncEntry, (StrT, arg_types)));
	arg_types.clear();

	//stringEqual
	arg_types.push_back(StrT);
	arg_types.push_back(StrT);
	nameTable.put(Symbol::symbol("stringEqual"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//stringLen
	arg_types.push_back(StrT);
	nameTable.put(Symbol::symbol("stringLen"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//substring
	arg_types.push_back(StrT);
	arg_types.push_back(IntT);
	arg_types.push_back(IntT);
	nameTable.put(Symbol::symbol("substring"), gcnew(FuncEntry, (StrT, arg_types)));
	arg_types.clear();

	//stringConcat
	arg_types.push_back(StrT);
	arg_types.push_back(StrT);
	nameTable.put(Symbol::symbol("stringConcat"), gcnew(FuncEntry, (StrT, arg_types)));
	arg_types.clear();

	//not
	arg_types.push_back(IntT);
	nameTable.put(Symbol::symbol("not"), gcnew(FuncEntry, (IntT, arg_types)));
	arg_types.clear();

	//getchar
	nameTable.put(Symbol::symbol("getchar"), gcnew(FuncEntry, (StrT, arg_types)));
	arg_types.clear();

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
		//unsigned int id = entry.first;
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
	Frame *frame = Frame::newFrame(Symbol::symbol(ENTRY_POINT_LABEL_NAME), formals);
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

void codegenPhase2(const assem::InstructionList &instList, Frame *frame, std::string *out);

void codegenPhase(const FragmentList &frags, std::string *out)
{
	printSource();

	std::string dataOut;
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
			BasicBlocks bb(stms, frame->getEndFuncLabel());
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
			codegenPhase2(instList, frame, out);
		} else {
			assert(frag->isData());
			DataFragment *data = (DataFragment*)frag;
			dataOut += data->toString();
			dataOut += "\n";
			DBG("%d", data->toString().c_str());
		}
		++it;
	}
	*out += dataOut;
}

void codegenPhase2(const assem::InstructionList &instList, Frame *frame, std::string *out)
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
	//flow.show();
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
		if (!s.empty()) {
			*out += s;
			*out += "\n";
		}
		++it;
	}
	*out += "\n\n";
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

	std::string out;
	out += ".text\n";
	out += "\t.align\t2\n";
	out += "\t.global\t__tigermain\n";

	codegenPhase(frags, &out);
	DBG("\n%s", out.c_str());

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
