#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int getlength( Node* root){
  if (root == nullptr){
    return 0; }
  // how long it is
  return 1 + max(getlength(root->left),  getlength(root->right) );}


bool equalPaths(Node * root)
{

  //recursive routine to see length of traversal
// compare max length of subtree of left and right 
    //work -post order to calc lenght = counter +1?
    //counter if length are equal 


// empty tree
if ( root == nullptr){return true;}

  //base - leaf node - end of tree
if (!root->left   && !root->right){return true;}

  
    //workk

    int leftlength = getlength(root->left);
    int rightlength = getlength(root->right); 

  if (root->left && root->right)
  //boolean answer
  //if both left and right give answer
        return (leftlength == rightlength);

        // only left - continue down the left path
        // to check entire tree correctly
    if (root->left) {
    return equalPaths(root->left);
    } else {
    return equalPaths(root->right);
    }


}

