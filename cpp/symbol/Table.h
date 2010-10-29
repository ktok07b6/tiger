#ifndef TABLE_H
#define TABLE_H

#include "Object.h"
#include <map>
#include <list>
#include <cassert>
#include "debug.h"
#include "HeapManager.h"

struct Symbol;


template<class T>
struct BindInScope : public Object
{
	typedef std::list<T*> TBinders;
	Symbol *key;
	T *value;
	TBinders *binds;
};



template<class T>
struct BindsInScope : public Object
{
	typedef std::list< BindInScope<T> > List;
	unsigned int id;
	List li;
};


template<class T>
struct Scopes : public Object
{
	typedef typename BindsInScope<T>::List List;
	typedef std::map<unsigned int, List> ScopeMap;
	typedef std::pair<unsigned int, List> ScopeMapEntry;

	void add(unsigned int id, List li) {
		dict.insert(std::make_pair(id, li));
	}
	const List *getScope(unsigned int id) {
		typename ScopeMap::iterator it = dict.find(id);
		if (it != dict.end()) {
			return &(it->second);
		} else {
			return NULL;
		}
	}
	ScopeMap dict;
};


template<class T>
class Table 
{
	typedef std::list<T*> TList;
	typedef std::map<Symbol*, TList*> BinderDictionary;
	typedef typename BinderDictionary::iterator BinderDictionaryIter;

	BinderDictionary dict;
	std::list<BindsInScope<T>*> bindsInScopeList;
	int scopeLevel;
	Scopes<T> &scopes;

 public:
 Table(Scopes<T> &s)
	 : scopeLevel(0)
		, scopes(s)
	{
		//beginScope(0);
	}

	~Table() {
		//FIXME:
		//BindsInScopeはObjectなのでHeapManagerから削除される
		//このためstatic globalなTableのデストラクタでは、
		//endScopeでのbindsInScopeListへのアクセスは不正なものとなる
		//endScope();
		typename BinderDictionary::const_iterator it;
		it = dict.begin();
		while(it != dict.end()) {
			TList *binds = it->second;
			delete binds;
			++it;
		}
	}

	T *get(Symbol *key) {
		DBG("get key = %s %p", key->name.c_str(), key);
		BinderDictionaryIter it;
		it = dict.find(key);
		if (it != dict.end()) {
			TList *binds = it->second;
			DBG("binds = %p", binds);
			T *value = binds->front();
			DBG("get value = %s %p", value->toString().c_str(), value);
			return value;
		} else {
			DBG("(nil)");
			return NULL;
		}
	}	

	void put(Symbol *key, T *value) 
	{
		DBG("put key = %s [%p]", key->name.c_str(), key);
		DBG("put value = %s [%p]", value->toString().c_str(), value);
		
		BinderDictionaryIter it;
		it = dict.find(key);
		TList *binds;
		if (it != dict.end()) {
			DBG("add binder entry");
			//変数束縛の上書きはBinderのリストで表現
			binds = it->second;
			binds->push_front(value);
			DBG("binds = %p", binds);
		} else {
			DBG("new binder entry");
			//1つのkeyにつき1つのBindersインスタンスを生成
			//Bindersが指すT *valueは
			//カレントスコープの変数束縛を表す
			binds = new TList();
			binds->push_front(value);
			DBG("binds = %p", binds);
			dict.insert(std::make_pair(key, binds));
		}
		assert(!bindsInScopeList.empty());
		BindsInScope<T> *scope = bindsInScopeList.back();
		BindInScope<T> scopeEntry;
		scopeEntry.key = key;
		scopeEntry.value = value;
		scopeEntry.binds = binds;
		scope->li.push_back(scopeEntry);
	}

	void concat(const Table<T> &other)
	{
		typename BinderDictionary::const_iterator it;
		it = other.dict.begin();
		while(it != other.dict.end()) {
			TList *binds = it->second;
			assert(binds->size() == 1);//1スコープ階層の定義のみ追加可能
			put(it->first, binds->front());
			++it;
		}
	}

	void beginScope(void *exp) 
	{
		DBG("%s", __PRETTY_FUNCTION__);
		BindsInScope<T> *scope = gcnew(BindsInScope<T>, ());
		scope->id = (unsigned int)exp;
		bindsInScopeList.push_back(scope);
		scopeLevel++;
	}

	void endScope() 
	{
		//DBG("%s", toString().c_str());
		DBG("%s", __PRETTY_FUNCTION__);
		--scopeLevel;
		assert(!bindsInScopeList.empty());
		BindsInScope<T> *scope = bindsInScopeList.back();
		bindsInScopeList.pop_back();

		//make scopesymbols & delete TList entry
		typename std::list< BindInScope<T> >::iterator it;
		it = scope->li.begin();
		while (it != scope->li.end()) {
			const BindInScope<T> &scopeEntry = *it;
			TList *binds = scopeEntry.binds;
			if (!binds->empty()) {
				binds->pop_front();
			}
			++it;
		}
		scopes.add(scope->id, scope->li);
		//delete scope;
		//DBG("%s", toString().c_str());
	}

	std::string toString() {
		std::string result = "\n";
		typename std::list<BindsInScope<T>*>::const_iterator it;
		it = bindsInScopeList.begin();
		int level = 0;
		while (it != bindsInScopeList.end()) {
			char buf[32];
			sprintf(buf, "scope %d ===========\n", level);
			result += buf;

			BindsInScope<T> *scope = *it;
			typename std::list< BindInScope<T> >::iterator it2;
			it2 = scope->li.begin();
			while (it2 != scope->li.end()) {
				const BindInScope<T> &scopeEntry = *it2;
				result += "\tkey: " + scopeEntry.key->name + "\n";
				result += "\tval: " + scopeEntry.value->toString() + "\n";
				++it2;
			}
			++it;
			++level;
			result += "====================\n";
		}
		return result;
	}
};

typedef Table<Absyn> SymbolTable;
typedef Table<Type> TypeTable;

#endif //TABLE_H
