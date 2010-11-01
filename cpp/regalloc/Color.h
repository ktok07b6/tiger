#ifndef COLOR_H
#define COLOR_H

#include "TempMap.h"
#include "Node.h"

namespace graph {
class InterferenceGraph;
}
namespace regalloc {

class Color : public TempMap
{
public:
	Color(const graph::InterferenceGraph &igraph);
	virtual std::string tempMap(Temp *temp);

private:
	void pushToSimplifyWorkList(graph::Node *node);
	enum {
		K = 8
	};

	graph::NodeList simplifyWorkList;
};

} //namespace regalloc
#endif
