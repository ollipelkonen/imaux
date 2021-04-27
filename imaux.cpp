#include <memory>
#include <vector>
#include <iostream>
#include <chrono>


//#define VALUE_TYPE double

//class Layer;

int numLayers = 0;
int numNodes = 0;
int numConnections = 0;

#include "layer.h"


std::chrono::high_resolution_clock::time_point t1, t2;

void timeStart()
{
  t1 = std::chrono::high_resolution_clock::now();
}

void timeEnd(std::string s)
{
  t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> time_span = t2 - t1;
  std::cout << "____  " << s << "  took " << time_span.count() << " ms\n";
}

int main()
{
  int size = 2;

  timeStart();
  auto d = std::make_unique<Layer<double>>(size);
  auto n = d->addChild( size*=2 );
  do {
    n = n->addChild( size*size );
    size *= 2;
  }
  while ( size < 64 );
  n = n->addChild( size*size );
  /*n = n->addChild( size*size );
  n = n->addChild( size*size );
  n = n->addChild( size*size );*/
  std::cout << "____ layers: " << numLayers << "\nnodes: " << numNodes << "\nnumConnections: " << numConnections << "\n";

  timeEnd("initialization");

  timeStart();
  d->test();
  timeEnd("test");

  timeStart();
  d->calc();
  timeEnd("calc");

  //auto m = d->getLast();
  auto m = d->next->next;
  for (auto const& nod: m->nodes)
  {
    std::cout << nod->value << "   ";
  }

  d->debugPrint();

  //delete d;
  return 0;
}
