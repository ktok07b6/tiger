#ifndef TEMP_MAP_H
#define TEMP_MAP_H

class Temp;

class TempMap
{
 public:
	virtual ~TempMap() {}
	virtual Temp *tempMap(Temp *temp) = 0;
};
#endif //TEMP_MAP_H

