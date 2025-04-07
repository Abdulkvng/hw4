#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{
}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{
}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}

/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/

template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    // Inserts a new item and does balancing magic
    virtual void insert (const std::pair<const Key, Value> &new_item);
    // Removes an item and fixes the tree (hopefully)
    virtual void remove(const Key& key);
    
protected:
    // Swaps two nodes and their balances (I think)
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Helper functions:
    void rebalance(AVLNode<Key, Value>* node);  // fixes unbalanced nodes
    void rotateLeft(AVLNode<Key, Value>* node);  // rotates left 
    void rotateRight(AVLNode<Key, Value>* node);  // rotates right 
    void updateBalanceFactors(AVLNode<Key, Value>* node);  // updates balances after changes
    void adjustAfterInsert(AVLNode<Key, Value>* node);  // called after inserting
    void adjustAfterRemove(AVLNode<Key, Value>* node);  // same but after removing
    int getNodeHeight(AVLNode<Key, Value>* node) const;  // gets height from node down
    int getBalanceFactor(AVLNode<Key, Value>* node) const;  // calculats balance
    void updateNodeHeight(AVLNode<Key, Value>* node);  // updates height and balance
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // First do the regular BST insertion
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if (this->root_ == nullptr) {
        this->root_ = newNode;
        return;}

AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
AVLNode<Key, Value>* parent = nullptr;

while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();} 
      else if (new_item.first > current->getKey()) {
            current = current->getRight();} else 
    {// if key already exists - update value
        current->setValue(new_item.second);
        delete newNode;
            return; } }

    newNode->setParent(parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode); } 
        else {parent->setRight(newNode); }

    // Update heights and balance factors, then rebalance if needed
    adjustAfterInsert(newNode);}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // First find the node to remove
    AVLNode<Key, Value>* nodeToRemove = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (nodeToRemove == nullptr) {
        return; }

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    // if the node has 2 children
    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        // Find predecessor
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        // Swap with predecessor
        nodeSwap(nodeToRemove, predecessor);
        parent = nodeToRemove->getParent();}

    AVLNode<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? 
                                nodeToRemove->getLeft() : nodeToRemove->getRight();

 if (parent == nullptr) {
    this->root_ = child;
    if (child != nullptr) {
        child->setParent(nullptr);
    }
} else {
    if (parent->getLeft() == nodeToRemove) {
        parent->setLeft(child);} 
        else {
        parent->setRight(child);}
    if (child != nullptr) {
        child->setParent(parent); }
}

    // Update heights and balance factors, then rebalance if needed
    if (parent != nullptr) { adjustAfterRemove(parent); }
 delete nodeToRemove;}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
   int8_t tmp = n1->getBalance();
n1->setBalance(n2->getBalance());
n2->setBalance(tmp);
    
    }

template<class Key, class Value>
int AVLTree<Key, Value>::getNodeHeight(AVLNode<Key, Value>* node) const
{
    if (node == nullptr) return -1;
    //similar to the 1st part of the assignment
return 1 + std::max(getNodeHeight(node->getLeft()), getNodeHeight(node->getRight()));}

template<class Key, class Value>
int AVLTree<Key, Value>::getBalanceFactor(AVLNode<Key, Value>* node) const
{
    if (node == nullptr) return 0;
    return getNodeHeight(node->getRight()) - getNodeHeight(node->getLeft());
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateNodeHeight(AVLNode<Key, Value>* node)
{
    if (node != nullptr) {
        node->setBalance(getBalanceFactor(node));
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::adjustAfterInsert(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
        updateNodeHeight(node);
        
        if (node->getBalance() > 1 || node->getBalance() < -1) {
    rebalance(node);
         break;}
        node = node->getParent(); }}

template<class Key, class Value>
void AVLTree<Key, Value>::adjustAfterRemove(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
updateNodeHeight(node);
        
        if (node->getBalance() > 1 || node->getBalance() < -1) {
    AVLNode<Key, Value>* parent = node->getParent();
    rebalance(node);
    node = parent;
        } else {
            node = node->getParent();} }}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    // Right heavy
    if (node->getBalance() > 1) {
        if (node->getRight()->getBalance() < 0) {
            // Right-left case
            rotateRight(node->getRight());
        }
        // Right-right case
        rotateLeft(node);
    } 
    // Left heavy
    else if (node->getBalance() < -1) {
        if (node->getLeft()->getBalance() > 0) {
            // Left-right case
            rotateLeft(node->getLeft());
        }
        // Left-left case
        rotateRight(node);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(newRoot->getLeft());
    if (newRoot->getLeft() != nullptr) {
        newRoot->getLeft()->setParent(node); }
    newRoot->setLeft(node);
    node->setParent(newRoot);

    newRoot->setParent(parent);
    if (parent == nullptr) {
        this->root_ = newRoot;
    } else if (parent->getLeft() == node) {
        parent->setLeft(newRoot);} 
        else {
        parent->setRight(newRoot);}

    // U
    updateNodeHeight(node);
    updateNodeHeight(newRoot);
}
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(newRoot->getRight());
    if (newRoot->getRight() != nullptr) {
        newRoot->getRight()->setParent(node);
    }

    newRoot->setRight(node);
    node->setParent(newRoot);

    newRoot->setParent(parent);
    if (parent == nullptr) {
        this->root_ = newRoot;
    } else if (parent->getLeft() == node) {
        parent->setLeft(newRoot);
    } else {
        parent->setRight(newRoot);
    }

    // basically Update    balance  after rotation
    updateNodeHeight(node);
    updateNodeHeight(newRoot);
}
#endif