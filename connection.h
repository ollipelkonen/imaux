#ifndef CONNECTION_H_INCLUDED
#define CONNECTION_H_INCLUDED


extern int numLayers;
extern int numNodes;
extern int numConnections;


class Connection
{
public:
  Connection() {
    numConnections++;
  }
  double weight = 0;
  double bias = 0;
  double value = 0;
};

#endif