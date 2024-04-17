/* Your code here! */
#include "dsets.h"
using namespace std;

void DisjointSets::addelements(int num) {
  for (int i = 0; i < num; i++) {
    vect.push_back(-1);
  }
}

int DisjointSets::find(int elem) {

    int root = elem;
    while (vect[root] >= 0) {
        root = vect[root];
    }

    while (elem != root) {
        int temp = vect[elem];
        vect[elem] = root;
        elem = temp;
    }
    
    return root;
}

void DisjointSets::setunion(int a, int b) {
  int rootA = find(a);
  int rootB = find(b);

  if (rootA == rootB) {
    return;
  }

  int newSize = vect[rootA] + vect[rootB];

    if (vect[rootA] <= vect[rootB]) {
        vect[rootB] = rootA;
        vect[rootA] = newSize;
    } else {
        vect[rootA] = rootB;
        vect[rootB] = newSize;
    }
}

int DisjointSets::size(int elem) {
   return vect[find(elem)] * -1;
}

