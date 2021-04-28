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

extern int numLayers;
extern int numNodes;
extern int numConnections;







template <class T> class Node
{
public:
  T value;
  std::vector<std::unique_ptr<Connection>> connections;
  Layer<T>& parent;

  Node( Layer<T>& parent ) : parent(parent) {
    numNodes++;
  }

  //void createConnections(Layer<T>& next);
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
  T sigmoid(T z)
  {
    return 1.0 / (1.0 + exp(-z));
    //return erf(sqrt(pi)*z/2); // much faster? uses <cmath>
    //return z / (1 + abs(z));  // fast, but not the same
  }

  T calc()
  {
    //TODO: according to this, sigmoid is calculated AFTER adds http://neuralnetworksanddeeplearning.com/chap2.html
    for (auto const& v: this->connections)
    {
      //v->value += sigmoid(value * v->weight + v->bias);
      v->value += value * v->weight + v->bias;
    }
    return 0;
  }
};


#endif