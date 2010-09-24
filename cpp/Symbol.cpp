#include "Symbol.h"
#include <map>
#include "debug.h"
#include "HeapManager.h"

using namespace std;

namespace {
	typedef map<string, Symbol*> SymbolDictionary;
	SymbolDictionary dict;
}

Symbol *
Symbol::symbol(const string &n)
{
	Symbol *s = NULL;
	SymbolDictionary::iterator it;
	it = dict.find(n);
	if (it != dict.end()) {
		s = it->second;
	} else {
		s = gcnew(Symbol, (n));
		dict.insert(make_pair(n, s));
	}

	DBG("name = %s, symbol = %p", n.c_str(), s);
	return s;
}
