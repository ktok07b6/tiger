#ifndef _TIGER_H_
#define _TIGER_H_

#include <set>
#include <string>

void initialize();
extern std::set<std::string> typeIDs;
bool isTypeID(const char*id);

std::string format(const char *fmt, ...);

#define ENTRY_POINT_LABEL_NAME "_start"
#define END_FUNCTION_LABEL_NAME "end_of_function"

#endif //_TIGER_H_
