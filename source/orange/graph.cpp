/*
    This file is part of Orange.

    Orange is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Orange is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Orange; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Authors: Janez Demsar, Blaz Zupan, 1996--2002
    Contact: janez.demsar@fri.uni-lj.si
*/


#include "graph.ppp"

#define GN_INIT \
  if ((v < 0) || (v >= nVertices)) \
    raiseError("vertex index %i is out of range 0-%i", v, nVertices-1); \
  neighbours.clear(); \
  \
  if (!directed) { \
    getNeighbours_Undirected(v, neighbours); \
    return; \
  }

#define GN_INIT_EDGETYPE \
  if ((v < 0) || (v >= nVertices)) \
    raiseError("vertex index %i is out of range 0-%i", v, nVertices-1); \
  if (edgeType >= nEdgeTypes) \
    raiseError("edge type %i is out of range 0-%i", v, nEdgeTypes-1); \
  neighbours.clear(); \
  \
  if (!directed) {\
    getNeighbours_Undirected(v, edgeType, neighbours); \
    return; \
  }



TGraph::TGraph(const int &nVert, const int &nTypes, const bool dir)
: nVertices(nVert),
  nEdgeTypes(nTypes),
  directed(dir),
  lastAddition(-1),
  lastRemoval(-1),
  currentVersion(0)
{
  if (nVertices<1)
    raiseError("invalid number of vertices (less than 1)");

  if (!nEdgeTypes)
    nEdgeTypes = 1;
  else if (nEdgeTypes < 0)
    raiseError("invalid (negative) number of edge types");
}


TGraphAsMatrix::TGraphAsMatrix(const int &nVert, const int &nTypes, const bool dir)
: TGraph(nVert, nTypes, dir)  
{
  const int &msize = nEdgeTypes * (directed ? nVertices * nVertices : (nVertices*(nVertices+1)) >> 1);
  edges = new float[msize];
  for(float *vt = edges, *ve = edges + msize; vt != ve; *(vt++) = GRAPH__NO_CONNECTION);
}

TGraphAsMatrix::~TGraphAsMatrix()
{ 
  delete edges;
}


float *TGraphAsMatrix::getEdge(const int &v1, const int &v2)
{ 
  float *edge = findEdge(v1, v2);
  for(float *e = edge, *ee = edge+nEdgeTypes; e!=ee; e++)
    if (*e != GRAPH__NO_CONNECTION)
      return edge;
  return NULL;
}
      

float *TGraphAsMatrix::findEdge(const int &v1, const int &v2)
{ 
  if (v1>v2)
    if ((v1>=nVertices) || (v2<0))
      raiseError("invalid vertex index (%i, %i)", v1, v2);
    else
      return edges + nEdgeTypes * (v2 + (directed ? v1*nVertices : ((v1*(v1+1))) >> 1));
  else
    if ((v2 >= nVertices) || (v1<0))
      raiseError("invalid vertex index (%i, %i)", v1, v2);
    else
      return edges + nEdgeTypes * (directed ? (v1*nVertices + v2) : (((v2*(v2+1)) >> 1) + v1) );

  return NULL; // only to make the compiler happy
}


float *TGraphAsMatrix::getOrCreateEdge(const int &v1, const int &v2)
{ 
  float *res = findEdge(v1, v2);
  lastAddition = ++currentVersion;
  return res;
}


void TGraphAsMatrix::removeEdge(const int &v1, const int &v2)
{
  float *edge = getEdge(v1, v2);
  if (edge) {
    for(float *eedge = edge + nEdgeTypes; edge != eedge; *(edge++) = GRAPH__NO_CONNECTION);
    lastRemoval = ++currentVersion;
  }
}


#define CHECK_NONEMPTY(weights) \
{ for(swe = (weights), ne = nEdgeTypes; ne-- && (*swe++ == GRAPH__NO_CONNECTION); ); \
  if (ne >= 0) { neighbours.push_back(v2); continue; } }


void TGraphAsMatrix::getNeighbours_Undirected(const int &v, vector<int> &neighbours)
{
  int v2 = 0, ne;
  float *swe;
  float *weights = edges + nEdgeTypes * ((v * (v+1)) >> 1);

  for(; v2<=v; weights += nEdgeTypes, v2++)
    CHECK_NONEMPTY(weights)

  // v2 now equals v+1 and weights points to the beginning of the row for v2
  for(weights += v * nEdgeTypes; v2 < nVertices; weights += ++v2 * nEdgeTypes)
    CHECK_NONEMPTY(weights)
}


void TGraphAsMatrix::getNeighbours(const int &v, vector<int> &neighbours)
{
  GN_INIT
  int v2 = 0, ne;
  float *swe;
  int llen = nVertices * nEdgeTypes;
  float *weightsFrom = edges + nEdgeTypes * v * nVertices;
  float *weightsTo = edges + nEdgeTypes * v;
  for(; v2 < nVertices; weightsFrom += nEdgeTypes, weightsTo += llen, v2++) {
    CHECK_NONEMPTY(weightsFrom)
    CHECK_NONEMPTY(weightsTo)
  }
}


void TGraphAsMatrix::getNeighboursFrom(const int &v, vector<int> &neighbours)
{
  GN_INIT
  getNeighboursFrom_Single(v, neighbours);
  int v2 = 0, ne;
  for(float *swe, *weights = edges + nEdgeTypes * v * nVertices; v2 < nVertices; weights += nEdgeTypes, v2++)
    CHECK_NONEMPTY(weights)
}


void TGraphAsMatrix::getNeighboursFrom_Single(const int &v, vector<int> &neighbours)
{
  neighbours.clear();
  int v2 = 0, ne;
  for(float *swe, *weights = edges + nEdgeTypes * (directed ? (v * nVertices) : ((v*(v+1)) >> 1)); v2 <= v; weights += nEdgeTypes, v2++)
    CHECK_NONEMPTY(weights)
}


void TGraphAsMatrix::getNeighboursTo(const int &v, vector<int> &neighbours)
{
  GN_INIT
  int v2 = 0, ne;
  int llen = nVertices * nEdgeTypes;
  for(float *swe, *weights = edges + nEdgeTypes * v; v2 < nVertices; weights += llen, v2++)
    CHECK_NONEMPTY(weights)
}

#undef CHECK_NONEMPTY


// A macro for checking the existence of certain type of connection,
// used for GraphAsMatrix (another macro with the same name is defined
// later, which works for other representations)

#define CHECK_EDGE(weights) \
  if (*(weights) != GRAPH__NO_CONNECTION) \
    neighbours.push_back((v2));

void TGraphAsMatrix::getNeighbours_Undirected(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  float *weights = edges + nEdgeTypes * ((v * (v+1)) >> 1) + edgeType, *we = weights + nEdgeTypes * (v+1);

  for(; weights != we; weights += nEdgeTypes, v2++)
    CHECK_EDGE(weights)

  // v2 now equals v+1 and weights points to the beginning of the row for v2
  for(; v2 < nVertices; weights += v2++ * nEdgeTypes)
    CHECK_EDGE(weights)
}


void TGraphAsMatrix::getNeighbours(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  float *weightsFrom = edges + nEdgeTypes * v * nVertices + edgeType;
  float *weightsTo = edges + nEdgeTypes * v + edgeType;
  for(; v2 < nVertices; weightsFrom += nEdgeTypes, weightsTo += nEdgeTypes * nVertices, v2++)
    if ((*weightsFrom != GRAPH__NO_CONNECTION) || (*weightsTo != GRAPH__NO_CONNECTION))
      neighbours.push_back(v2);
}


void TGraphAsMatrix::getNeighboursFrom(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  for(float *weights = edges + nEdgeTypes * v * nVertices + edgeType; v2 < nVertices; weights += nEdgeTypes, v2++)
    CHECK_EDGE(weights)
}


void TGraphAsMatrix::getNeighboursFrom_Single(const int &v, const int &edgeType, vector<int> &neighbours)
{
  neighbours.clear();
  int v2 = 0;
  for(float *weights = edges + nEdgeTypes * (directed ? (v * nVertices) : ((v*(v+1)) >> 1)) + edgeType; v2 <=v; weights += nEdgeTypes, v2++)
    CHECK_EDGE(weights)
}


void TGraphAsMatrix::getNeighboursTo(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  int llen = nVertices * nEdgeTypes;
  for(float *weights = edges + nEdgeTypes * v + edgeType; v2 < nVertices; weights += llen, v2++)
    CHECK_EDGE(weights)
}

#undef CHECK_EDGE

TGraphAsList::TEdge *TGraphAsList::createEdge(TEdge *next, const int &vertex)
{
  TEdge *newedge = (TEdge *)malloc(sizeof(TEdge) + (nEdgeTypes-1)*sizeof(float));
  newedge->next = next;
  newedge->vertex = vertex;
  float *w = &newedge->weights;
  for(int i = nEdgeTypes; i--; *w++ = GRAPH__NO_CONNECTION);
  return newedge;
}



TGraphAsList::TGraphAsList(const int &nVert, const int &nEdge, const bool dir)
: TGraph(nVert, nEdge, dir),
  edges(new TEdge *[nVert])
{
  TEdge **e = edges;
  for(int i = nVert; i--; *e++ = NULL);
}


TGraphAsList::~TGraphAsList()
{
  TEdge **e = edges;
  for(int i = nVertices; i--; e++)
    for(TEdge *ei = *e, *en; ei; ei = en) {
      en = ei->next;
      delete ei;
    }
  delete edges;
}


bool TGraphAsList::findEdgePtr(const int &v1, const int &v2, TEdge **&e, int &subvert)
{
  if (directed) {
    if ((v1 >= nVertices) || (v1 < 0))
      raiseError("vertex index %i is out of range 0-%i", v1, nVertices-1);
    if ((v2 >= nVertices) || (v2 < 0))
      raiseError("vertex index %i is out of range 0-%i", v2, nVertices-1);

    e = edges + v1;
    subvert = v2;
  }
  else {
    if (v1<v2) {
      if ((v2>=nVertices) || (v1<0))
        raiseError("invalid vertex index (%i, %i)", v1, v2);

      e = edges + v2;
      subvert = v1;
    }

    else {
      if ((v1>=nVertices) || (v2<0))
        raiseError("invalid vertex index (%i, %i)", v1, v2);

      e = edges + v1;
      subvert = v2;
    }
  }
  
  while(*e)
    if ((*e)->vertex >= subvert)
      return (*e)->vertex == subvert;
    else
      e = &((*e)->next);

  return false;
}


float *TGraphAsList::getEdge(const int &v1, const int &v2)
{
  TEdge **e;
  int subvert;
  return findEdgePtr(v1, v2, e, subvert) ? &((*e)->weights) : NULL;
}


float *TGraphAsList::getOrCreateEdge(const int &v1, const int &v2)
{
  TEdge **e;
  int subvert;
  if (!findEdgePtr(v1, v2, e, subvert))
    *e = createEdge(*e, subvert);

  return &((*e)->weights);
}


void TGraphAsList::removeEdge(const int &v1, const int &v2)
{
  TEdge **e;
  int subvert;
  if (findEdgePtr(v1, v2, e, subvert)) {
    TEdge *n = (*e)->next;
    delete *e;
    *e = n;
  }
}

void TGraphAsList::getNeighbours_Undirected(const int &v, vector<int> &neighbours)
{
  TEdge *e;
  for(e = edges[v]; e; e = e->next)
    neighbours.push_back(e->vertex);

  if (!directed) {
    int v2 = v+1;
    for(TEdge **se = edges+v2, **ee = edges+nVertices; se != ee; v2++, se++) {
      for(e = *se; e && (e->vertex <=v); e = e->next)
        if (e->vertex == v) {
          neighbours.push_back(v2);
          break;
        }
    }
  }
}


void TGraphAsList::getNeighbours(const int &v, vector<int> &neighbours)
{
  GN_INIT

  // passes through the v's list and the edges simultaneously and merges the neighbours to get a sorted list
  int lastV = -1;
  for(TEdge *e = edges[v]; e; lastV = e->vertex, e = e->next) {
    for(int v2 = lastV; ++v2 != e->vertex; ) {
      for(TEdge *e2 = edges[v2]; e2 && (e2->vertex <=v); e2 = e2->next)
        if (e2->vertex == v) {
          neighbours.push_back(v2);
          break;
        }
    }

    neighbours.push_back(e->vertex);
  }
}


void TGraphAsList::getNeighboursFrom(const int &v, vector<int> &neighbours)
{
  GN_INIT
  getNeighboursFrom_Single(v, neighbours);
}


void TGraphAsList::getNeighboursFrom_Single(const int &v, vector<int> &neighbours)
{
  neighbours.clear();
  for(TEdge *e = edges[v]; e; e = e->next)
    neighbours.push_back(e->vertex);
}


void TGraphAsList::getNeighboursTo(const int &v, vector<int> &neighbours)
{
  GN_INIT
  int v2 = 0;
  for(TEdge **se = edges, **ee = edges+nVertices; se != ee; v2++, se++) {
    for(TEdge *e = *se; e && (e->vertex <=v); e = e->next)
      if (e->vertex == v) {
        neighbours.push_back(v2);
        break;
      }
  }
}


// A macro for checking the existence of certain type of connection,
// useful for GraphAsList (this is not the same as the ones defined
// for GraphAsMatrix and for GraphAsTree)

#define CHECK_EDGE(e,v2) { if ((&(e)->weights)[edgeType] != GRAPH__NO_CONNECTION) neighbours.push_back((v2)); }
#define CHECK_EDGE_TO(e) { if (e->vertex==v) { CHECK_EDGE(e, v2); break; }}

void TGraphAsList::getNeighbours_Undirected(const int &v, const int &edgeType, vector<int> &neighbours)
{
  TEdge *e;
  for(e = edges[v]; e; e = e->next)
    CHECK_EDGE(e, e->vertex);

  int v2 = v+1;
  for(TEdge **se = edges+v2, **ee = edges+nVertices; se != ee; v2++, se++)
    for(e = *se; e && (e->vertex <=v); e = e->next)
      CHECK_EDGE_TO(e)
}




void TGraphAsList::getNeighbours(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE

  // passes through the v's list and the edges simultaneously and merges the neighbours to get a sorted list
  int lastV = -1;
  for(TEdge *e = edges[v]; e; lastV = e->vertex, e = e->next) {
    for(int v2 = lastV; ++v2 != e->vertex; )
      for(TEdge *e2 = edges[v2]; e2 && (e2->vertex <=v); e2 = e2->next)
        CHECK_EDGE_TO(e2)
    CHECK_EDGE(e,e->vertex)
  }
}


void TGraphAsList::getNeighboursFrom(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  getNeighboursFrom_Single(v, edgeType, neighbours);
}


void TGraphAsList::getNeighboursFrom_Single(const int &v, const int &edgeType, vector<int> &neighbours)
{
  neighbours.clear();
  for(TEdge *e = edges[v]; e; e = e->next)
    CHECK_EDGE(e, e->vertex)
}


void TGraphAsList::getNeighboursTo(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  for(TEdge **se = edges, **ee = edges+nVertices; se != ee; v2++, se++)
    for(TEdge *e = *se; e && (e->vertex <=v); e = e->next)
      CHECK_EDGE_TO(e)
}

#undef CHECK_EDGE
#undef CHECK_EDGE_TO


TGraphAsTree::TEdge *TGraphAsTree::createEdge(const int &vertex)
{
  TEdge *newedge = (TEdge *)malloc(sizeof(TEdge) + (nEdgeTypes-1)*sizeof(float));
  newedge->vertex = vertex;
  newedge->left = newedge->right = NULL;
  float *w = &newedge->weights;
  for(int i = nEdgeTypes; i--; *w++ = GRAPH__NO_CONNECTION);
  return newedge;
}


TGraphAsTree::TEdge::~TEdge()
{
  if (left)
    delete left;
  if (right)
    delete right;
}

TGraphAsTree::TGraphAsTree(const int &nVert, const int &nEdge, const bool dir)
: TGraph(nVert, nEdge, dir),
  edges(new TEdge *[nVert])
{
  TEdge **e = edges;
  for(int i = nVert; i--; *e++ = NULL);
}


TGraphAsTree::~TGraphAsTree()
{
  TEdge **e = edges;
  for(int i = nVertices; i--; e++)
    if (*e)
      delete *e;
  delete edges;
}



void TGraphAsTree::sortIndices(const int &v1, const int &v2, TEdge **&e, int &subvert) const
{
  if (directed) {
    if ((v1 >= nVertices) || (v1 < 0))
      raiseError("vertex index %i is out of range 0-%i", v1, nVertices-1);
    if ((v2 >= nVertices) || (v2 < 0))
      raiseError("vertex index %i is out of range 0-%i", v2, nVertices-1);

    e = edges + v1;
    subvert = v2;
  }
  else {
    if (v1<v2) {
      if ((v2>=nVertices) || (v1<0))
        raiseError("invalid vertex index (%i, %i)", v1, v2);

      e = edges + v2;
      subvert = v1;
    }

    else {
      if ((v1>=nVertices) || (v2<0))
        raiseError("invalid vertex index (%i, %i)", v1, v2);

      e = edges + v1;
      subvert = v2;
    }
  }
}

float *TGraphAsTree::getEdge(TEdge *node, const int &subvert)
{
  while(node) {
    const int nvert = (int)(node->vertex & 0x7fffffff);
    if (nvert == subvert)
      return &node->weights;
    node = subvert < nvert ? node->left : node->right;
  }

  return NULL;
}


float *TGraphAsTree::getEdge(const int &v1, const int &v2)
{
  TEdge **pnode;
  int subvert;
  sortIndices(v1, v2, pnode, subvert);
  return getEdge(*pnode, subvert);
}


#define PAINT_RED(x) (x)->vertex |= 0x80000000
#define PAINT_BLACK(x) (x)->vertex &= 0x7fffffff
#define IS_RED(x) ((x)->vertex > 0x7fffffff)
#define IS_BLACK(x) ((x)->vertex < 0x80000000)

float *TGraphAsTree::getOrCreateEdge(const int &v1, const int &v2)
{
  TEdge **node;
  int subvert;
  sortIndices(v1, v2, node, subvert);

  vector<TEdge **> stack;

  stack.push_back(node);

  while(*node) {
    const int nvert = (int)((*node)->vertex & 0x7fffffff);
    if (nvert == subvert)
      return &((*node)->weights);
    node = &(subvert < nvert ? (*node)->left : (*node)->right);
    stack.push_back(node);
  }

  vector<TEdge **>::const_iterator sptr(stack.end()-1), stop(stack.begin());
  **sptr = createEdge(subvert | 0x80000000);
  float *res = &((***sptr).weights);

  // while the current node's parent is red
  while((sptr!=stop) && IS_RED(*sptr[-1])) { 

    // this node has no grandparent -- just paint the father black
    if (sptr - stop == 1) {
      PAINT_BLACK(**stop);
      break;
    }

    #define TROOT *sptr[-2]
    TEdge * const grandParent = *sptr[-2];
    TEdge * const parent = *sptr[-1];
    TEdge * const node = **sptr;

    // this node has a red uncle -- paint the grandfather red and parent and uncle black, handle the grandfather
    TEdge * const uncle = (sptr[-1] == &(grandParent->left)) ? grandParent->right : grandParent->left;
    if (uncle && IS_RED(uncle)) {
      PAINT_RED(grandParent);
      PAINT_BLACK(grandParent->left);
      PAINT_BLACK(grandParent->right);
      sptr -= 2; // now let's deal with the grandparent
      continue;
    }

    // if this node's parent is the left child of its grandparent...
    if (sptr[-1] == &(grandParent->left))

      // if this node is the left child of its parent
      if (*sptr == &(parent->left)) {
        grandParent->left = parent->right; // watchout: this overrides *sptr[-1] (it is stored in parent, though)
        parent->right = grandParent;
        TROOT = parent;

        PAINT_RED(grandParent);
        PAINT_BLACK(parent);
        break;
      }

      // if this node is the right child of its parent
      else {
        parent->right = node->left;
        grandParent->left = node->right;
        node->left = parent;
        node->right = grandParent;
        TROOT = node;

        PAINT_BLACK(node);
        PAINT_RED(grandParent);
        break;
      }

    // if this node's parent is the right child of its grandparent
    else

      // if this node is the right child of its parent
      if (*sptr == &(parent->right)) {
        grandParent->right = parent->left; // watchout: this overrides *sptr[-1] (it is stored in parent, though)
        parent->left = grandParent;
        TROOT = parent;

        PAINT_RED(grandParent);
        PAINT_BLACK(parent);
        break;
      }

      // if this node is the leftchild of its parent
      else {
        parent->left = node->right;
        grandParent->right = node->left;
        node->right = parent;
        node->left = grandParent;
        TROOT = node;

        PAINT_BLACK(node);
        PAINT_RED(grandParent);
        break;
      }
  }

  return res;
}


void TGraphAsTree::removeEdge(const int &v1, const int &v2)
{
  TEdge **node;
  int subvert;
  sortIndices(v1, v2, node, subvert);

  vector<TEdge **> stack;

  stack.push_back(node);

  while(*node) {
    const int nvert = (int)((*node)->vertex & 0x7fffffff);
    if (nvert == subvert)
      break;
    node = &(subvert < nvert ? (*node)->left : (*node)->right);
    stack.push_back(node);
  }

  if (!*node)
    return;

  TEdge *temp = *node;

  // find the bigest element in the left subtree
  if ((*node)->left) {
    node = &(*node)->left;
    stack.push_back(node);

    while ((*node)->right) {
      node = &(*node)->right;
      stack.push_back(node);
    }
  }

  if (*node != temp) {
    temp->vertex = (temp->vertex & 0x80000000) | ((*node)->vertex & 0x7fffffff);
    memcpy(&temp->weights, &(*node)->weights, nEdgeTypes * sizeof(float));
  }

  bool removedRed = IS_RED(*node);

  temp = *node;
  *node = (*node)->left ? (*node)->left : (*node)->right;
  temp->left = temp->right = NULL;
  delete temp;

  if (removedRed)
    return;

  vector<TEdge **>::iterator sptr(stack.end()-1), stop(stack.begin());
  
  for(;;) {
    TEdge *node = **sptr;

    if (node && IS_RED(node)) {
      PAINT_BLACK(node);
      return;
    }

    if (sptr == stop)
      return;

    TEdge *parent = *sptr[-1];
    
    if (*sptr == &(parent->left)) {
      TEdge *brother = parent->right;

      // Kononenko, case C1: brother is red, thus nephews are black; rotate so that one nephew becomes a brother
      if (IS_RED(brother)) {
        *sptr[-1] = brother;
        parent->right = brother->left;
        brother->left = parent;

        PAINT_BLACK(brother);
        PAINT_RED(parent);

        // fix the stack
        int sindex = sptr - stop;
        *sptr = &(brother->left);
        stack.push_back(&(parent->left));
        // push_back might have invalidated sptr
        stop = stack.begin();
        sptr = stop+sindex+1;

        brother = parent->right;
      }

      // Kononenko, case C2: brother is black and has no red nephews; paint him red and some node upwards will have to become black
      if ((!brother->left || IS_BLACK(brother->left)) && (!brother->right || IS_BLACK(brother->right))) {
        PAINT_RED(brother);
        sptr--;
        continue;
      }

      // Kononenko, case C3.1: brother is black and the outer nephew is black: bring the inner (which is surely red) out
      // This does not follow the book -- it does Fig 23a and 23b in one step
      if (!brother->right || IS_BLACK(brother->right)) {
        TEdge *nephew = brother->left;
        *sptr[-1] = nephew;
        parent->right = nephew->left;
        brother->left = nephew->right;
        nephew->left = parent;
        nephew->right = brother;

        if IS_RED(parent)
          PAINT_BLACK(parent);
        else
          PAINT_BLACK(nephew);

        return;
      }

      // Kononenko, case C3a: brother is black and the outer nephew is red
      parent->right = brother->left;
      brother->left = parent;
      *sptr[-1] = brother;

      if IS_RED(parent) {
        PAINT_RED(brother);
        PAINT_BLACK(parent);
      }
      PAINT_BLACK(brother->right);

      return;
    }
     
    else {
      TEdge *brother = parent->left;

      // Kononenko, case C1: brother is red, thus nephews are black; rotate so that one nephew becomes a brother
      if (IS_RED(brother)) {
        *sptr[-1] = brother;
        parent->left = brother->right;
        brother->right = parent;

        PAINT_BLACK(brother);
        PAINT_RED(parent);

        // fix the stack
        int sindex = sptr - stop;
        *sptr = &(brother->right);
        stack.push_back(&(parent->right));
        // push_back might have invalidated sptr
        stop = stack.begin();
        sptr = stop+sindex+1;

        brother = parent->left;
      }

      // Kononenko, case C2: brother is black and has no red nephews; paint him red and some node upwards will have to become black
      if ((!brother->left || IS_BLACK(brother->left)) && (!brother->right || IS_BLACK(brother->right))) {
        PAINT_RED(brother);
        sptr--;
        continue;
      }

      // Kononenko, case C3.1: brother is black and the outer nephew is black: bring the inner (which is surely red) out
      // This does not follow the book -- it does Fig 23a and 23b in one step
      if (!brother->left || IS_BLACK(brother->left)) {
        TEdge *nephew = brother->right;
        *sptr[-1] = nephew;
        parent->left = nephew->right;
        brother->right = nephew->left;
        nephew->right = parent;
        nephew->left = brother;

        if IS_RED(parent)
          PAINT_BLACK(parent);
        else
          PAINT_BLACK(nephew);
        return;
      }

      // Kononenko, case C3a: brother is black and the outer nephew is red
      parent->left = brother->right;
      brother->right = parent;
      *sptr[-1] = brother;

      if IS_RED(parent) {
        PAINT_RED(brother);
        PAINT_BLACK(parent);
      }
      PAINT_BLACK(brother->left);

      return;
    }
  }
}


void TGraphAsTree::getNeighbours_fromTree(TEdge *edge, vector<int> &neighbours)
{
  if (edge->left)
    getNeighbours_fromTree(edge->left, neighbours);

  neighbours.push_back(int(edge->vertex & 0x7fffffff));

  if (edge->right)
    getNeighbours_fromTree(edge->right, neighbours);
}



void TGraphAsTree::getNeighbours_Undirected(const int &v, vector<int> &neighbours)
{
  if (edges[v])
    getNeighbours_fromTree(edges[v], neighbours);

  int v2 = v+1;
  for(TEdge **ee = edges+v+1; v2 < nVertices; v2++, ee++)
    if (*ee && getEdge(*ee, v))
      neighbours.push_back(v2);
}



void TGraphAsTree::getNeighbours_fromTree_merge(TEdge *edge, vector<int> &neighbours, const int &v, int &latest)
{
  const int thisVertex = int(edge->vertex & 0x7fffffff);

  if (edge->left)
    getNeighbours_fromTree_merge(edge->left, neighbours, v, latest);

  for(TEdge **node = edges + ++latest; latest < thisVertex; latest++, node++)
    if (*node && getEdge(*node, v))
      neighbours.push_back(latest);
  
  neighbours.push_back(thisVertex);

  if (edge->right)
    getNeighbours_fromTree_merge(edge->right, neighbours, v, latest);
}

void TGraphAsTree::getNeighbours(const int &v, vector<int> &neighbours)
{
  GN_INIT

  int latest = -1;
  if (edges[v])
    getNeighbours_fromTree_merge(edges[v], neighbours, v, latest);

  for(TEdge **node = edges + ++latest; latest < nVertices; latest++, node++)
    if (*node && getEdge(*node, v))
      neighbours.push_back(latest);
}


void TGraphAsTree::getNeighboursFrom(const int &v, vector<int> &neighbours)
{
  GN_INIT
  getNeighboursFrom_Single(v, neighbours);
}


void TGraphAsTree::getNeighboursFrom_Single(const int &v, vector<int> &neighbours)
{
  neighbours.clear();
  if (edges[v])
    getNeighbours_fromTree(edges[v], neighbours);
}


void TGraphAsTree::getNeighboursTo(const int &v, vector<int> &neighbours)
{
  GN_INIT
  int v2 = 0;
  for(TEdge **ee = edges; v2 < nVertices; v2++, ee++)
    if (*ee && getEdge(*ee, v))
      neighbours.push_back(v2);
}



// A macro for checking the existence of certain type of connection,
// useful for GraphAsTree (this is not the same as the ones defined
// for GraphAsMatrix and for GraphAsList)

#define CHECK_EDGE(e,v2) { if ((&(e)->weights)[edgeType] != GRAPH__NO_CONNECTION) neighbours.push_back((v2)); }
#define CHECK_EDGE_TO(v2) { \
  float *w = *node ? getEdge(*node,v) : NULL; \
  if (w && (w[edgeType] != GRAPH__NO_CONNECTION)) \
    neighbours.push_back(v2); \
  }

void TGraphAsTree::getNeighbours_fromTree(TEdge *edge, const int &edgeType, vector<int> &neighbours)
{
  if (edge->left)
    getNeighbours_fromTree(edge->left, edgeType, neighbours);

  CHECK_EDGE(edge, int(edge->vertex & 0x7fffffff));

  if (edge->right)
    getNeighbours_fromTree(edge->right, edgeType, neighbours);
}


void TGraphAsTree::getNeighbours_Undirected(const int &v, const int &edgeType, vector<int> &neighbours)
{
  getNeighbours_fromTree(edges[v], edgeType, neighbours);

  int v2 = v+1;
  for(TEdge **node = edges+v+1; v2 < nVertices; v2++, node++) 
    CHECK_EDGE_TO(v2)
}



void TGraphAsTree::getNeighbours_fromTree_merge(TEdge *edge, const int &edgeType, vector<int> &neighbours, const int &v, int &latest)
{
  const int thisVertex = int(edge->vertex & 0x7fffffff);

  if (edge->left)
    getNeighbours_fromTree_merge(edge->left, edgeType, neighbours, v, latest);

  for(TEdge **node = edges + ++latest; latest < thisVertex; latest++, node++)
    CHECK_EDGE_TO(latest)
  
  CHECK_EDGE(edge, thisVertex)

  if (edge->right)
    getNeighbours_fromTree_merge(edge->right, edgeType, neighbours, v, latest);
}


void TGraphAsTree::getNeighbours(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE

  int latest = -1;
  if (edges[v])
    getNeighbours_fromTree_merge(edges[v], edgeType, neighbours, v, latest);

  for(TEdge **node = edges + ++latest; latest < nVertices; latest++)
    CHECK_EDGE_TO(latest)
}


void TGraphAsTree::getNeighboursFrom(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  getNeighboursFrom_Single(v, edgeType, neighbours);
}


void TGraphAsTree::getNeighboursFrom_Single(const int &v, const int &edgeType, vector<int> &neighbours)
{
  neighbours.clear();
  if (edges[v])
    getNeighbours_fromTree(edges[v], edgeType, neighbours);
}


void TGraphAsTree::getNeighboursTo(const int &v, const int &edgeType, vector<int> &neighbours)
{
  GN_INIT_EDGETYPE
  int v2 = 0;
  for(TEdge **node = edges; v2 < nVertices; v2++, node++)
    CHECK_EDGE_TO(v2)
}
