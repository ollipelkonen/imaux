#ifndef LAYER_H_INCLUDED
#define LAYER_H_INCLUDED


#include <memory>
#include <vector>
#include <iostream>

#include "node.h"

extern int numLayers;
extern int numNodes;
extern int numConnections;


extern void    AddLog(const char* fmt, ...) IM_FMTARGS(2);


#ifndef Node
template <class T> class Node;
#endif

template <class T>
class Layer : std::enable_shared_from_this<Layer<T>>
{
public:
  int size;
  std::vector<std::shared_ptr<Node<T>>> nodes;
  std::shared_ptr<Layer<T>> next = NULL;
  //std::weak_ptr<Layer<T>> previous = NULL;
  Layer<T> *previous = NULL;

  ~Layer()
  {
  }

  Layer(int size) {
    numLayers++;
    this->size = size;
    for (int i=0; i<size; i++)
    {
      this->nodes.push_back( std::make_shared<Node<T>>( *this ) );
    }
    //std::cout << "create node " << numLayers << "  size " << size << "   nodes: " << numNodes << "   connections " << numConnections << "\n";
    AddLog( "create node %i   size %i   nodes: %i   connections: %i", numLayers, size, numNodes, numConnections );
  }

  void clear()
  {
    //std::cout << "  clear layer of size " << size << "\n";
    AddLog( "  clear layer of size %i", size );
    for( auto const &n: this->nodes )
      n->clear();
    if ( this->next != NULL )
      next->clear();
  }

  void calc()
  {
    if ( previous == NULL )
      clear();
    //std::cout << "  calc layer of size " << size << " / " << nodes.size() << "\n";
    AddLog("  calc layer of size %i", nodes.size());
    for( auto const &n: this->nodes )
      n->calc();

    if ( this->next != NULL )
    {
      for ( int i=0; i<this->next->size; i++ )
      {
        T v = 0;
        for( int j=0; j<this->size; j ++ )
          v += this->nodes.at(j)->connections.at(i)->value;
        auto const & node = this->next->nodes.at(i);
        //node->value = v;
        //TODO: here or in node?
        node->value = sigmoid(v);
      }
      next->calc();
    }
  }


  T sigmoid(T z)
  {
    return 1.0 / (1.0 + exp(-z));
  }

  Layer<T>* getLast()
  {
    if ( next != NULL )
      return next->getLast();
    return this;
  }

  void debugPrint()
  {
    //std::cout << "value: " << nodes.at(0)->value << "\n";
    AddLog("value: %f", nodes.at(0)->value);
    if ( next != NULL )
      next->debugPrint();
  }

  auto addChild( int size )
  {
    this->next = std::make_shared<Layer<T>>(size);
    for( auto const & n: this->nodes )
      n->createConnections(*this->next);
    //next->previous = std::enable_shared_from_this<Layer<T>>::shared_from_this();
    next->previous = this;
    return this->next;
  }

  void test(int n = 0 )
  {
    auto node = nodes.at(0);
    //std::cout << "testing layer " << n << ": nodes " << nodes.size() << "   conns: " << (node->connections.size() * nodes.size()) << "\n";
    AddLog( "testing layer %i:  nodes: %i  conns: %i", n, nodes.size(), (node->connections.size() * nodes.size()) );
    if ( this->next != NULL )
      this->next->test(n+1);
  }
};




#endif