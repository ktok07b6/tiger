#include "tiger.h"

std::set<std::string> typeIDs;

void initialize()
{
	typeIDs.insert("int");
	typeIDs.insert("string");
}

bool isTypeID(const char*id)
{
	return (typeIDs.find(id) != typeIDs.end());
}
