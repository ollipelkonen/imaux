#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED


extern volatile int numLayers;
extern volatile int numNodes;
extern volatile int numConnections;


class Connection
{
public:
  Connection(double w=0, double b=0) : weight(w), bias(b) {
    numConnections++;
  }
  double weight = 0;
  double bias = 0;
  double value = 0;
};

#endif