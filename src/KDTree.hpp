// Copyright

#ifndef SRC_KDTREE_HPP_
#define SRC_KDTREE_HPP_

#include <cmath>
#include <iostream>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>
#include "Point.hpp"


using namespace std;
template <typename value_type>
class KDTreeNode{
public: 
  value_type nodeValue;
  KDTreeNode<value_type>* nextNodes[2];
  KDTreeNode(const value_type& _nodeValue){
    nodeValue = _nodeValue;
    nextNodes[0] = 0;
    nextNodes[1] = 0;
  }
  KDTreeNode(value_type _nodeValue, KDTreeNode<value_type>* Lnode, KDTreeNode<value_type>* Rnode){
    nodeValue = _nodeValue;
    nextNodes[0] = Lnode;
    nextNodes[1] = Rnode;
  }
};

template <size_t N, typename ElemType>
class KDTree {
 public:
  typedef pair<Point<N>, ElemType> value_type;

  KDTree();

  ~KDTree();

  KDTree(const KDTree &rhs);
  KDTree &operator=(const KDTree &rhs);

  size_t dimension() const;

  size_t size() const;
  bool empty() const;

  bool contains(const Point<N> &pt) const;

  void insert(const Point<N> &pt, const ElemType &value);
  
  ElemType &operator[](const Point<N> &pt);

  ElemType &at(const Point<N> &pt);
  const ElemType &at(const Point<N> &pt) const;

  //Add
  bool find(const Point<N>& pt, KDTreeNode<value_type>**& ptrNode) const;
 void knnIterator(Point<N> key, KDTreeNode<value_type>* currentNode, vector<pair<ElemType, double>>& vecContent, int level) const;
    ElemType knn_value(const Point<N>& key, size_t k) const;
    vector<ElemType> knn_query(const Point<N>& key, size_t k) const;
 private:
  KDTreeNode<value_type>* headNode= nullptr;
  size_t dimension_;
  size_t size_;
};

//functions
template <typename value_type>
void killNodes(KDTreeNode<value_type>* node){
  if(node != nullptr){
    killNodes((node->nextNodes)[0]);
    killNodes((node->nextNodes)[1]);
    delete node;
  }
}

template <typename value_type>
KDTreeNode<value_type>* initNode(const KDTreeNode<value_type>* tempNode){
  const KDTreeNode<value_type>* LtempNode=tempNode->nextNodes[0];
  const KDTreeNode<value_type>* RtempNode=tempNode->nextNodes[1];
  const  value_type tempValue= tempNode->nodeValue;
  KDTreeNode<value_type>* nodeCopy=nullptr;
  if (tempNode != nullptr) {
    nodeCopy = new KDTreeNode<value_type>(tempValue, initNode(LtempNode), initNode(RtempNode));
  }
  return nodeCopy;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::find(const Point<N>& pt, KDTreeNode<value_type>**& ptrNode) const {
  size_t iterator = 0;
  ptrNode = const_cast<KDTreeNode<value_type>**> (&headNode);
  for ( ; *ptrNode and ((*ptrNode)->nodeValue).first != pt;
    ptrNode = &((*ptrNode)->nextNodes[pt[iterator % dimension_] > (((*ptrNode)->nodeValue).first)[iterator % dimension_]]))
      {iterator++;}
  return *ptrNode != 0;
}
//endfunctions

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
  dimension_ = N;
  size_ = 0;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
  killNodes(headNode);
  headNode=nullptr;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(const KDTree& rhs) {
  headNode = initNode(rhs.headNode);
  dimension_ = rhs.dimension_;
  size_ = rhs.size_;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree& rhs) {
  headNode = initNode(rhs.headNode);
  dimension_ = rhs.dimension_;
  size_ = rhs.size_;
  return *this;
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
  return dimension_;
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {
  return size_;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const {
  if(headNode==nullptr) return true;
  else return false;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const {
  KDTreeNode<value_type>** ptrNode;
  if (!find(pt, ptrNode)) return false;
  else return true;
}
template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value) {
  KDTreeNode<value_type>** ptrNode;
  if (!find(pt, ptrNode)) {
    value_type valueNew;
    valueNew.first = pt;
    valueNew.second = value;
    *ptrNode = new KDTreeNode<value_type>(valueNew);
    size_ +=1;
  }
  ((*ptrNode)->nodeValue).second = value;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& pt) {
  KDTreeNode<value_type>** ptrNode;
  if (!find(pt, ptrNode)) {
    value_type valueNew;
    valueNew.first = pt;
    valueNew.second = size_;
    *ptrNode = new KDTreeNode<value_type>(valueNew);
    return ((*ptrNode)->nodeValue).second;
  }
  return ((*ptrNode)->nodeValue).second;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::at(const Point<N>& pt){
  KDTreeNode<value_type>** ptrNode;
  if (find(pt, ptrNode))
      return ((*ptrNode)->nodeValue).second;
  throw out_of_range("out_of_range");
}

template <size_t N, typename ElemType>
const ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) const{
  at(pt);
}

//KNN_generic
template <size_t N, typename ElemType>
void KDTree<N, ElemType>::knnIterator(Point<N> key, KDTreeNode<value_type>* tempNode, vector<pair<ElemType,double>>& vecContent,int level) const{
  if (tempNode == nullptr) return; 
  double kdDistance = distance((tempNode->nodeValue).first, key);
  vecContent.push_back(make_pair((tempNode->nodeValue).second, kdDistance));
  knnIterator(key, (tempNode->nextNodes)[0], vecContent,++level);
  knnIterator(key, (tempNode->nextNodes)[1], vecContent,++level);
}

template <size_t N, typename ElemType>
vector<ElemType> KDTree<N, ElemType>::knn_query(const Point<N>& key, size_t k) const{
    vector<pair<ElemType,double>> vecContent;
    vector<ElemType> query;
    KDTreeNode<value_type>* tempNode = headNode;
    knnIterator(key, tempNode, vecContent,0);
    sort(vecContent.begin(), vecContent.end(), [](const auto& x, const auto& y) { return x.second < y.second; });
    for (size_t i = 0; i < k; i++) query.push_back(vecContent[i].first);

    return query;
}

template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::knn_value(const Point<N>& key, size_t k) const {
  if (k > size_) k = size_;
  bool condition;
  vector<ElemType> values(k);
  values = knn_query(key, k);
  vector<ElemType> vecET;
  vector<pair<ElemType,size_t>> vecET_S;
  pair<ElemType, size_t> top = vecET_S[0];
  vecET.push_back(values[0]);
  for (size_t i = 1; i < k; i++) {
    condition = false;
    for (size_t j = 0; j < vecET.size(); j++) {
      if (values[i] == vecET[j]) condition = true;
    }
    if (!condition) vecET.push_back(values[i]);
  }
  //create ElemType,size_T
  for (size_t i = 0; i < vecET.size(); i++) vecET_S.push_back(make_pair(vecET[i], 0));
  //pass all vec
  for (size_t i = 0; i < k; i++) {
    for (size_t j = 0; j < vecET_S.size(); j++) {
      if (values[i] == vecET_S[j].first)
        vecET_S[j].second ++;
    }
  }
  //top pair 
  for (size_t i = 1; i < vecET_S.size(); i++) {
    if (top.second < vecET_S[i].second)
      top = vecET_S[i];
  }
  return top.first;
}

#endif  // SRC_KDTREE_HPP_