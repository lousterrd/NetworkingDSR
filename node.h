#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <time.h>
using namespace std;

struct requestCheck {
  char startRREQ;
  int reqidRREQ;
};

class Node {
public:
  Node(char nodeChar, map<char, Node*> connections);
  Node* memoryLoc();
  Node& operator=(const Node& other);

  bool receivedPath;
  bool receivedResponse;vector<requestCheck> prevRequests;
  string RREQ;
  string RREP;
  char nodeName;

  void insertConnection(char nodeName, Node* location);
  void createConnections(map<char, Node*> connections);
  void removeConnection(char nodeName);
  map<char, Node*> nodeConnections;
  bool isConnected(char nodeName);
  void sendRREP(char src, char dst, string RREQreply, int size, char RREPsrc);

};


Node::Node(char nodeChar, map<char, Node*> connections) {
  nodeName = nodeChar;
  createConnections(connections);
  receivedPath = false;
  receivedResponse = false;
}

Node* Node::memoryLoc() {
  return this;
}

Node& Node::operator=(const Node& rhs) {
  if (this == &rhs)
    return *this;
  return *this;
}

void Node::createConnections(map<char, Node*> connections) {
  nodeConnections = connections;
}

void Node::insertConnection(char nodeName, Node* location) {
  nodeConnections.insert(pair<char, Node*>(nodeName, location));
}

void Node::removeConnection(char nodeName) {
  nodeConnections.erase(nodeName);
}

bool Node::isConnected(char nodeName) {
  if (nodeConnections.count(nodeName) > 0)
    return true;
  else
    return false;
}

void Node::sendRREP(char src, char dst, string RREQreply, int size, char RREPsrc) {
  if (size == 0) {
    cout << nodeName << " got path to " << RREPsrc << " from:"
          << RREQreply << RREPsrc << endl;
  }
  else if (size == (RREQreply.size()-1)) {
    cout << nodeName << " got RREP from " << RREPsrc << " to "
          << dst << " from: " << RREQreply << RREPsrc << endl;
  }
  else if (size != RREQreply.size()) {
    cout << nodeName << " got RREP from " << RREQreply[size+1] << " to "
          << dst << " from: " << RREQreply << RREPsrc << endl;
  }
  else {}

  // send the next RREP
  receivedResponse = true;
  map<char, Node*>::iterator findIterator;
  Node* nextNode = NULL;
  if ((size-1) >= 0) {
    if(size != 0) {
      findIterator = nodeConnections.find(RREQreply[size-1]);
    }
    nextNode = findIterator->second;
  }
  if (nextNode != NULL) {
    nextNode -> RREP = RREQreply;
    nextNode -> sendRREP(nextNode->nodeName, dst, RREQreply, size-1, RREPsrc);
  }
}
