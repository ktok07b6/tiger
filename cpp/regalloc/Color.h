#ifndef COLOR_H
#define COLOR_H

#include "TempMap.h"
#include "Node.h"
#include <stack>
#include "InterferenceGraph.h"
namespace graph {
	//class InterferenceGraph;
}

namespace regalloc {

class Color : public TempMap
{
public:
	Color(const graph::InterferenceGraph &igraph);
	virtual std::string tempMap(Temp *temp);

private:

	void coloring();
	void coalesce();
	bool setColor(graph::Node *);
	int getColoredIndex(graph::Node *);
	bool isEnableColoring() const;

	void pushToSimplifyWorks(graph::Node *node);
	graph::Node *popFromSimplifyWorks();

	enum {
		K = 8
	};

	std::stack<graph::Node*> simplifyWorks;
	graph::InterferenceGraph igraph;
	graph::NodeList coloredNodes[K];
};

} //namespace regalloc
#endif
