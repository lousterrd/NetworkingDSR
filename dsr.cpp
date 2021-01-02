// Louis Doherty and Drew Morri
#include "node.h"
#include <time.h>
using namespace std;

const int NUM_NODES = 9;

struct twoNodes {
  char node1;
  char node2;
};

// Picks two nodes at random to delete
twoNodes randomNodes(char nodes[]) {
  twoNodes chosenNodes;
  chosenNodes.node1 = nodes[rand()%NUM_NODES];
  chosenNodes.node2 = nodes[rand()%NUM_NODES];

  while(chosenNodes.node1 == chosenNodes.node2) {
    chosenNodes.node1 = nodes[rand()%NUM_NODES];
    chosenNodes.node2 = nodes[rand()%NUM_NODES];
  }
  return chosenNodes;
}

// Returns a pointer to a node when provided with the node's name
Node* nodePtr(char node, map<char, Node*> nodeMap) {
  map<char, Node*>::iterator findIterator = nodeMap.find(node);
	return (findIterator->second);
}

// Adds a connection between two different nodes
void addEdge(char node1, char node2, map<char, Node*> nodeMap) {
	Node* node1Ptr = nodePtr(node1, nodeMap);
	Node* node2Ptr = nodePtr(node2, nodeMap);

	node1Ptr->insertConnection(node2, node2Ptr);
	node2Ptr->insertConnection(node1, node1Ptr);
}

// Destroys a connection between two different nodes
void destroyEdge(char node1, char node2, map<char, Node*> nodeMap) {
	nodePtr(node1, nodeMap)->removeConnection(node2);
	nodePtr(node2, nodeMap)->removeConnection(node1);
}

// Returns true if there is a connection between two nodes, false if not
bool connection(char node1, char node2, map<char, Node*> nodeMap) {
	return nodePtr(node1, nodeMap)->isConnected(node2);
}

void sendRREQ(map<char, Node*> nodeMap, char src, int requestID, char dst, char nodes[]) {
  time_t start = time(0);
  Node* current = nodePtr(src, nodeMap);
  current->receivedPath = true;

  vector<char> neighbor;
  while(1)
  {
    bool check = true;

    for(int i=0; i < current->prevRequests.size(); i++) {
      if (current->prevRequests[i].startRREQ == src
            && current->prevRequests[i].reqidRREQ == requestID)
      {
          check = false;
      }
    }

    if (check) {
      requestCheck insertRec = {src, requestID};
      current->prevRequests.push_back(insertRec);
      if(current->nodeName != dst) {
        for (map<char,Node*>::const_iterator it = current->nodeConnections.begin(); it != current->nodeConnections.end(); it++) {
          Node* neighborNode = it->second;
          if (neighborNode->receivedPath == false) {
            neighborNode->RREQ = (current->RREQ + (current->nodeName));
            neighborNode->receivedPath = true;
            if (neighborNode->nodeName == dst) {
              cout << neighborNode->nodeName << " received RREQ from " << current->nodeName
                    << " to go to this node. Start RREP: "
                    << (neighborNode->RREQ + neighborNode->nodeName) << endl;
              neighborNode->sendRREP(dst, src, neighborNode->RREQ, (neighborNode->RREQ).size(), dst);
            }
            else {
              cout << neighborNode->nodeName << " received RREQ from " << current->nodeName
                    << ", with path: " << neighborNode->RREQ << endl;
            }
          }
          neighbor.push_back(it->first);
        }
      }
      else {}
    }
    if (neighbor.size() > 0) {
      current = nodePtr(neighbor[0], nodeMap);
      neighbor.erase(neighbor.begin());
    }
    if (difftime(time(0), start) > 1.0) {
      Node* verify = nodePtr(src, nodeMap);
      if(!verify->receivedResponse) {
        cout << "No path between " << src << " and " << dst << endl;
      }
      break;
    }
  }

  Node* resetNode = NULL;
  for (int i = 0; i < NUM_NODES; i++) {
    resetNode = nodePtr(nodes[i], nodeMap);
    resetNode->receivedPath = false;
    resetNode->RREP = "";
    resetNode->RREQ = "";
  }
  Node* resetSource = nodePtr(src, nodeMap);
  resetSource->receivedPath = false;
  resetSource->RREP = "";
  resetSource->RREQ = "";
}


int main() {
  char nodes[NUM_NODES] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I'};

  map<char, Node*> empty;
  map<char, Node*> nodeMap;

  Node A('A', empty);
  Node B('B', empty);
  Node C('C', empty);
  Node D('D', empty);
  Node E('E', empty);
  Node F('F', empty);
  Node G('G', empty);
  Node H('H', empty);
  Node I('I', empty);

  nodeMap.insert(pair<char, Node*>('A', A.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('B', B.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('C', C.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('D', D.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('E', E.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('F', F.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('G', G.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('H', H.memoryLoc()));
  nodeMap.insert(pair<char, Node*>('I', I.memoryLoc()));


  addEdge('A', 'B', nodeMap);
  addEdge('A', 'G', nodeMap);
  addEdge('B', 'D', nodeMap);
  addEdge('C', 'E', nodeMap);
  addEdge('C', 'F', nodeMap);
  addEdge('D', 'E', nodeMap);
  addEdge('D', 'H', nodeMap);
  addEdge('E', 'G', nodeMap);
  addEdge('E', 'H', nodeMap);
  addEdge('E', 'I', nodeMap);
  addEdge('F', 'I', nodeMap);


  cout << "This is the simulation, going from A to I: " << endl << endl;
	sendRREQ(nodeMap, 'A', rand(), 'I', nodes);
	cout << endl << "Destroyed the edge between A and G." << endl << endl;
	destroyEdge('A', 'G', nodeMap);
	cout << "Try to find new path between A to I:" << endl << endl;
	sendRREQ(nodeMap, 'A', rand(), 'I', nodes);
	cout << endl << "Restored the edge between A and G." << endl << endl;
	addEdge('A', 'G', nodeMap);

  cout << "Retrying the path from A to I to see if it's restored." << endl<<endl;
	sendRREQ(nodeMap, 'A', rand(), 'I', nodes);

  return 0;
}
