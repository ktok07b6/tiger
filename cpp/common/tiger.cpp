#include "tiger.h"
#include <stdarg.h>
#include <stdio.h>

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

namespace {
char buffer[4096];
}
std::string format(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsprintf(buffer, fmt, ap);
	return buffer;
}
