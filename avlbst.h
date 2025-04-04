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
    virtual void insert (const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key& key);
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Helper functions
    void rebalance(AVLNode<Key, Value>* node);
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    // might be hard too implement
    void updateBalanceAfterInsert(AVLNode<Key, Value>* node, const Key& key);
    void updateBalanceAfterRemove(AVLNode<Key, Value>* node);
    int getHeight(AVLNode<Key, Value>* node) const;
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
        return; }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    while (current != nullptr) {
        parent = current;
        if (new_item.first < current->getKey()) {
            current = current->getLeft();
        } else if (new_item.first > current->getKey()) {
            current = current->getRight();
        } else {
            // if key already exist -  update value
            current->setValue(new_item.second);
            delete newNode;
            return; }
    }

    newNode->setParent(parent);
    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);
    }

    // Update balance factors and rebalance if needed
    updateBalanceAfterInsert(parent, new_item.first);
}

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
        return; 
    }

    AVLNode<Key, Value>* parent = nodeToRemove->getParent();

    // if the node has two children
    if (nodeToRemove->getLeft() != nullptr && nodeToRemove->getRight() != nullptr) {
        // then F=ind predecessor
        AVLNode<Key, Value>* predecessor = static_cast<AVLNode<Key, Value>*>(this->predecessor(nodeToRemove));
        // Swap with predecessor
        nodeSwap(nodeToRemove, predecessor);
        parent = nodeToRemove->getParent();  }

    AVLNode<Key, Value>* child = (nodeToRemove->getLeft() != nullptr) ? 
                                nodeToRemove->getLeft() : nodeToRemove->getRight();

    if (child != nullptr) {
        child->setParent(parent); }

    if (parent == nullptr) {
        this->root_ = child; } 
        else if (parent->getLeft() == nodeToRemove) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    // Update balance factors and rebalance if needed
    if (parent != nullptr) {
        updateBalanceAfterRemove(parent); }

delete nodeToRemove;}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    std::swap(n1->balance_, n2->balance_);
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceAfterInsert(AVLNode<Key, Value>* node, const Key& key)
{
    while (node != nullptr) {
        if (key < node->getKey()) {
            node->updateBalance(-1);
        } else {
            node->updateBalance(1);}

        if (node->getBalance() == 0) {
            break;} 
        else if (abs(node->getBalance()) > 1) {
            rebalance(node);
            break; }
        node = node->getParent(); }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceAfterRemove(AVLNode<Key, Value>* node)
{
    while (node != nullptr) {
        int oldBalance = node->getBalance();
        
        if (abs(oldBalance) == 1) {
            break;
        }

    if (node->getLeft() && node->getRight()) {
            node->setBalance(0); } 
     
    else if (node->getLeft()) {
        node->setBalance(-1);
        } else if (node->getRight()) {
            node->setBalance(1);
        } else {
          node->setBalance(0);
        }
  if (abs(node->getBalance()) > 1) {  rebalance(node);}

        node = node->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rebalance(AVLNode<Key, Value>* node)
{
    if (node->getBalance() > 1) {
        if (node->getRight()->getBalance() < 0) {
            rotateRight(node->getRight());
        }
        rotateLeft(node);
    } else if (node->getBalance() < -1) {
        if (node->getLeft()->getBalance() > 0) {
            rotateLeft(node->getLeft());
        }
        rotateRight(node);}
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
    
    if (parent == nullptr) {
        this->root_ = newRoot;
    } else if (parent->getLeft() == node) {
        parent->setLeft(newRoot);
    } else {
        parent->setRight(newRoot);}
    newRoot->setParent(parent);
    
    // Update balance factors
    node->setBalance(node->getBalance() - 1 - std::max<int8_t>(0, newRoot->getBalance()));
    newRoot->setBalance(newRoot->getBalance() - 1 + std::min<int8_t>(0, node->getBalance()));
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* newRoot = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();
    
    node->setLeft(newRoot->getRight());
    if (newRoot->getRight() != nullptr) {
        newRoot->getRight()->setParent(node); }
    
    newRoot->setRight(node);
    node->setParent(newRoot);
    
    if (parent == nullptr) {
        this->root_ = newRoot;}
     else if (parent->getLeft() == node) {
        parent->setLeft(newRoot); } 
        else
     {parent->setRight(newRoot);  }
    newRoot->setParent(parent);
    
    // this should update the  balance factors - debug to see if it works 
    node->setBalance(node->getBalance() + 1 - std::min<int8_t>(0, newRoot->getBalance()));
    newRoot->setBalance(newRoot->getBalance() + 1 + std::max<int8_t>(0, node->getBalance()));
}

#endif