/**
 * File: KDTree.h
 * Author: (JunJie)
 * ------------------------
 * An interface representing a kd-tree in some number of dimensions. The tree
 * can be constructed from a set of data and then queried for membership and
 * nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include "Point.h"
#include "BoundedPQueue.h"
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <iostream>

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template<size_t N, typename ElemType>
struct KDTreeNode{
    Point<N> pointObj;
    KDTreeNode<N, ElemType>* left;
    KDTreeNode<N, ElemType>* right;
    ElemType value;
};

template <size_t N, typename ElemType>
class KDTree {
public:
    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree();
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree();
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& rhs);
    KDTree<N, ElemType>& operator=(const KDTree& rhs);
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const;
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const;
    bool empty() const;
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const;
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value);
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt);
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
    // TODO: Add implementation details here.
    size_t capacity;
    size_t count;
    KDTreeNode<N, ElemType> *head;
};

/** KDTree class implementation details */

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
    this->capacity = N;
    this->count = 0;
    head = nullptr;
}

template <size_t N, typename ElemType>
void helper(KDTreeNode<N, ElemType> *curr) {
    if(curr != nullptr){
        helper(curr->left);
        helper(curr->right);
        delete curr;
    }
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    helper(this->head);
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    return this->capacity;
}

// TODO: finish the implementation of the rest of the KDTree class
template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const{
    return this->count;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const{
    return this->count == 0;
}


template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const{
    KDTreeNode<N, ElemType> *curr = this->head;
    int index = 0;
    while(curr != nullptr){
        Point<N>* currPoint = &curr->pointObj;
        if(*currPoint == pt){
            return true;
        }
        else if(pt[index] < (*currPoint)[index]) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
        index = (index + 1) % this->capacity;
    }
    return false;
}


template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value){
    if(this->head ==  nullptr) {
        KDTreeNode<N, ElemType>* temp = new KDTreeNode<N, ElemType>;
        copy(pt.begin(), pt.end(), temp->pointObj.begin());
        temp->left = nullptr;
        temp->right = nullptr;
        temp->value = value;
        this->head = temp;
        this->count ++;
        return;
    }
    KDTreeNode<N, ElemType> *prev = nullptr;
    KDTreeNode<N, ElemType> *curr = this->head;
    int prevIndex;
    int index = 0;
    while(curr != nullptr){
        Point<N>* currPoint = &curr->pointObj;
        prev = curr;
        if(*currPoint == pt){
            curr->value = value;
            return;
        }
        else if(pt[index] < (*currPoint)[index]) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
        prevIndex = index;
        index = (index + 1) % this->capacity;
    }

    if(prev->pointObj[prevIndex] > pt[prevIndex]) {
        KDTreeNode<N, ElemType>* temp = new KDTreeNode<N, ElemType>;
        copy(pt.begin(), pt.end(), temp->pointObj.begin());
        temp->left = nullptr;
        temp->right = nullptr;
        temp->value = value;
        prev->left = temp;
        this->count ++;
    }
    else {
        KDTreeNode<N, ElemType>* temp = new KDTreeNode<N, ElemType>;
        copy(pt.begin(), pt.end(), temp->pointObj.begin());
        temp->left = nullptr;
        temp->right = nullptr;
        temp->value = value;
        prev->right = temp;
        this->count ++;
    }
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& pt){
    KDTreeNode<N, ElemType> *curr = this->head;
    int index = 0;
    while(curr != nullptr){
        Point<N>* currPoint = &curr->pointObj;
        if(*currPoint == pt){
            return curr->value;
        }
        else if(pt[index] < (*currPoint)[index]) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
        index = (index + 1) % this->capacity;
    }
    this->insert(pt, ElemType());
    return this->at(pt);
}


template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::at(const Point<N>& pt){
    KDTreeNode<N, ElemType> *curr = this->head;
    int index = 0;
    while(curr != nullptr){
        Point<N>* currPoint = &curr->pointObj;
        if(*currPoint == pt){
            return curr->value;
        }
        else if(pt[index] < (*currPoint)[index]) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
        index = (index + 1) % this->capacity;
    }
    throw std::out_of_range("Element is not in the tree.");
}

template <size_t N, typename ElemType>
const ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) const{
    KDTreeNode<N, ElemType> *curr = this->head;
    int index = 0;
    while(curr != nullptr){
        Point<N>* currPoint = &curr->pointObj;
        if(*currPoint == pt){
            return curr->value;
        }
        else if(pt[index] < (*currPoint)[index]) {
            curr = curr->left;
        }
        else {
            curr = curr->right;
        }
        index = (index + 1) % this->capacity;
    }
    throw std::out_of_range("Element is not in the tree.");
}

template <size_t N, typename ElemType>
void helper(KDTreeNode<N, ElemType> *curr, int index,int size, const Point<N>& key, BoundedPQueue<ElemType> &result) {
    if(curr == nullptr){
        return;
    }
    double dist = Distance(curr->pointObj, key);
    result.enqueue(curr->value, dist);

    int left = 1;
    if(curr->pointObj[index] <= key[index]) {
        left = 0;
        helper(curr->right, (index + 1)%N, size, key, result);
    }
    else {
        helper(curr->left, (index + 1)%N, size, key, result);
    }

    if(result.size() != size || abs(curr->pointObj[index] - key[index]) < result.worst()){
        if(left){
            helper(curr->right, (index + 1)%N, size, key, result);
        }
        else {
            helper(curr->left, (index + 1)%N, size, key, result);
        }
    }
}

template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const{

    BoundedPQueue<ElemType> result(k);
    helper(this->head, 0, k, key, result);
    unordered_map<ElemType, int> freq;
    ElemType final;
    int max = 0;
    while(result.size() > 0){
        ElemType curr = result.dequeueMin();
        freq[curr] += 1;
        if(freq[curr] > max){
            max = freq[curr];
            final = curr;
        }
    }

    return final;
}


template <size_t N, typename ElemType>
void cphelper(KDTreeNode<N, ElemType>*& ptr, const KDTreeNode<N, ElemType>* rhs) {
    if(rhs == nullptr) {
        ptr = nullptr;
        return;
    }

    Point<N> temp;
    copy(rhs->pointObj.begin(), rhs->pointObj.end(), temp.begin());
    KDTreeNode<N, ElemType>* tp = new KDTreeNode<N, ElemType>;
    tp->value = rhs->value;
    tp->pointObj = temp;
    ptr = tp;
    cphelper(tp->left, rhs->left);
    cphelper(tp->right, rhs->right);
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(const KDTree& rhs) {
    if(rhs.dimension() == N){
        count = rhs.count;
        capacity = rhs.capacity;
        cphelper(head, rhs.head);
    }
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree& rhs) {
    KDTree<N, ElemType> temp = rhs;
    if(rhs.dimension() == N){
        this->count = temp.count;
        this->capacity = temp.capacity;
        helper(this->head);
        cphelper(this->head, temp.head);
        return *this;
    }
    return *this;;
}



#endif // KDTREE_INCLUDED
