#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <memory>
#include <vector>
#include "layer.h"
#include "connection.h"

// exp from some of these
#include <math.h>
#include <cmath>
#include <complex>
#include <ctgmath>
#include <valarray>


#ifndef Layer
template <class T> class Layer;
#endif

extern volatile int numLayers;
extern volatile int numNodes;
extern volatile int numConnections;
extern void    AddLog(const char* fmt, ...) IM_FMTARGS(2);





template <class T> class Node
{
public:
  T value;
  std::vector<std::unique_ptr<Connection>> connections;
  Layer<T>& parent;

  Node( Layer<T>& parent ) : parent(parent) {
    numNodes++;
  }

  void createConnections(Layer<T>& next)
  {
    for (int i=0; i<next.size; i++)
    {
      this->connections.push_back( std::make_unique<Connection>() );
    }
  }

  void clear()
  {
    //std::cout << "  clear node of size " << connections.size() << "\n";
    for (auto const &c: this->connections)
      c->value = 0;
  }

  //T sigmoid(T z);
  T sigmoid(T z) const
  {
    return 1.0 / (1.0 + exp(-z));
    //return erf(sqrt(pi)*z/2); // much faster? uses <cmath>
    //return z / (1 + abs(z));  // fast, but not the same
  }

  void calc()
  {
    //TODO: check std::valarray, expression templates or https://github.com/blitzpp/blitz
    for (auto const& v: this->connections)
    {
      v->value += value * v->weight + v->bias;
    }
  }

  void calcError(std::vector<double>& target)
  {
    //TODO:
      for (auto const& v: this->connections)
    {
      v->value += value * v->weight + v->bias;
    }
  }

  void calcError()
  {
    //TODO:
    for (auto const& v: this->connections)
    {
      v->value += value * v->weight + v->bias;
    }
  }

};


#endif