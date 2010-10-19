#ifndef TEMP_MAP_H
#define TEMP_MAP_H

#include <string>
class Temp;

class TempMap
{
 public:
	virtual std::string tempMap(Temp *temp) = 0;
};
#endif //TEMP_MAP_H

