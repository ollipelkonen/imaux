#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED


extern int numLayers;
extern int numNodes;
extern int numConnections;


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