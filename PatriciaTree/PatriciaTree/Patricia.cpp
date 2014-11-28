#include <iostream>
#include <stack>
#include <fstream>
#include <cassert>
using namespace std;

#include "Patricia.h"

Patricia_Tree::Patricia_Tree(TData* junk)
{
    m_root = NULL;           // set root to NULL (empty tree)
    m_dummy = new Node();    // create new dummy node
    m_dummy->m_value = junk; // set dummy's data to 0;
    m_node_count = 0;
}

bool Patricia_Tree::getBitAtPos(char* key, int position)
{
    return (*(key+position / 8) & (1 << position % 8));
}

int Patricia_Tree::getFirstDiffBitPos(char* key1, char* key2)
{
    int position = 256*8 + 1;  // max number of bits allowed for input
    // 9999 = 14 bits (1-14) -> offset so (0-13)
    
    while ((getBitAtPos(key1, position) == getBitAtPos(key2, position)) && position >= 0)
    {
        position--;     // both are the same so compare at next MSB
    }
    
    return position;    // return -1 if both are duplicates
    // else return differing bit position
}

bool Patricia_Tree::insert(TData* data)
{
    bool insertion = false;
    int returnedBitCheck = 0;
    
    if (!m_root)
    {
        m_root = insert(data, returnedBitCheck); // returns pointer to root
        insertion = true;
    }
    else insertion = insert(m_root, data, returnedBitCheck); // returns true/false
    
    if (insertion == true)
    {
        cout << "Successful insertion of data with node bit test " << returnedBitCheck << endl;
        return true;
    }
    else
    {
        cout << "Illegal attempt to insert duplicate value" << endl;
        return false;
    }
}

bool Patricia_Tree::lookup(char* key)
{
    bool search = false;
    if (m_root) search = lookup(m_root, key);
    
    if (search == true)
    {
        cout << "Value found" << endl;
        return true;
    }
    else
    {
        cout << "Value not found" << endl;
        return false;
    }
}

bool Patricia_Tree::remove(char* key)
{
    bool removed = false;
    if (m_root) removed = remove(m_root, key);
    
    if (removed == true)
    {
        cout << "Successful deletion of" << endl;
        return true;
    }
    else
    {
        cout << "Illegal attempt to delete a value not present" << endl;
        return false;
    }
    
}

// insert function to be called for 1st insertion into empty tree
Patricia_Tree::Node *Patricia_Tree::insert(TData* data, int &returnedBitCheck)
{
    m_root = new Node();
    int diffBit = getFirstDiffBitPos(data->Key, m_dummy->m_value->Key);
    m_root->m_bit_check = diffBit;
    m_root->m_value = data;
    m_root->m_left = m_dummy;
    m_root->m_right = m_root;
    m_node_count++;
    returnedBitCheck = m_root->m_bit_check;
    return m_root;
}

bool Patricia_Tree::insert(Node *root, TData* data, int &returnedBitCheck)
{
    int prevBitTest = 0;
    int currBitTest = -1;
    stack<Node *> ptr_stack;    // stack to push/pop pointers of visited nodes
    
    while (currBitTest < prevBitTest && root != m_dummy)        // traverse the tree (keeping track of bit checks) until
        // the current bit check is greater or equal to the previous
        // bit check AND the root is not at the dummy node
    {
        ptr_stack.push(root);
        prevBitTest = root->m_bit_check;                        // keep track of the node predecessor's bit check
        int bitTest = getBitAtPos(data->Key, root->m_bit_check);    // check value's bit at the current Node's bit check position
        if (bitTest == 1) root = root->m_right;                 // if 1, traverse to right child; if 0, traverse to left child
        else root = root->m_left;
        currBitTest = root->m_bit_check;                        // keep track of current node's bit check
    }
    
    int diffBit = getFirstDiffBitPos(data->Key, root->m_value->Key);     // full key compare. If -1, value is a duplicate value and not to
    // to be inserted. Else, diffBit is the first position of differing
    // bits between value and the current node's bit check
    if (diffBit == -1)  // duplicate value
        return false;
    
    else    // there is a higher order bit which differs between the two
    {
        Node *predecessor = ptr_stack.top();
        ptr_stack.pop();
        Node *newNode = new Node();
        newNode->m_value = data;
        newNode->m_bit_check = diffBit;
        
        if (root == m_dummy && diffBit < predecessor->m_bit_check)  // CASE: insert at dummy node
        {
            // insert with the newly inserted node's left (zero) ptr pointing to the dummy
            newNode->m_left = m_dummy;
            newNode->m_right = newNode;
            predecessor->m_left = newNode;
            m_node_count++;
            returnedBitCheck = newNode->m_bit_check;
            return true;
        }
        
        if (diffBit > predecessor->m_bit_check) // INSERT AS NEW ROOT OR A TWEENER
        {
            // pop the stack until predcecessor's bit check is greater or equal
            // to current node's bit check or until you reach the root
            
            Node *childPred = predecessor;  // as the predecessor "moves up the tree" as the stack
            // is popped, childPred is the node that predecessor used
            // to point to
            
            while (ptr_stack.size() > 0 && predecessor->m_bit_check < diffBit)
            {
                childPred = predecessor;
                predecessor = ptr_stack.top();
                ptr_stack.pop();
            }
            
            // if predecessor is at the root and predecessor's bit check is less than diffBit
            if (ptr_stack.size() == 0 && predecessor->m_bit_check < diffBit)    // new root case; predecessor is at the root
            {
                newNode->m_left = predecessor;  // old root always becomes newNode's (the new root) left child
                newNode->m_right = newNode;
                m_root = newNode;
                m_node_count++;
                
                returnedBitCheck = newNode->m_bit_check;
                return true;
            }
            assert (predecessor->m_bit_check >= diffBit);
            // insert as a tweener!
            
            int bitTest = getBitAtPos(newNode->m_value->Key, newNode->m_bit_check);  // get the new node's own bit check
            
            if (bitTest == 1)
            {
                newNode->m_right = newNode;
                newNode->m_left = childPred;
            }
            else
            {
                newNode->m_left = newNode;
                newNode->m_right = childPred;
            }
            
            // rewire
            if (predecessor->m_left == childPred)
                predecessor->m_left = newNode;
            else if (predecessor->m_right == childPred)
                predecessor->m_right = newNode;
            
            m_node_count++;
            returnedBitCheck = newNode->m_bit_check;
            return true;
        }
        
        if (diffBit < predecessor->m_bit_check) // INSERT AS A LEAF
        {
            int bitTest = getBitAtPos(newNode->m_value->Key, newNode->m_bit_check);  // (value, diffBit)
            
            // if it's got a 1 at its bit check, then its 1 ptr points to itself
            // if it's got a 0 at its bit check, then its 0 ptr points to itself
            if (bitTest == 1)
            {
                if (predecessor->m_right->m_bit_check < predecessor->m_bit_check
                    || predecessor->m_left->m_bit_check < predecessor->m_bit_check) // has a true child
                {
                    newNode->m_right = newNode;
                    newNode->m_left = predecessor;
                    
                    // rewire
                    if (predecessor->m_right == predecessor)
                        predecessor->m_right = newNode;
                    else if (predecessor->m_left == predecessor)
                        predecessor->m_left = newNode;
                }
                // it's newNode's m_left because its bitTest == 1
                // (so it's right pointer will be pointing to itself)
                else if (predecessor->m_left == predecessor)
                {
                    newNode->m_left = predecessor->m_right;
                    newNode->m_right = newNode; // point newNode's 1 (right) pointer to itself
                    
                    // rewire
                    predecessor->m_right = newNode;
                }
                else if (predecessor->m_right == predecessor)
                {
                    newNode->m_left = predecessor->m_left;
                    newNode->m_right = newNode; // point newNode's 1 (right) pointer to itself
                    
                    // rewire
                    predecessor->m_left = newNode;
                }
            }
            else
            {
                if (predecessor->m_right->m_bit_check < predecessor->m_bit_check
                    || predecessor->m_left->m_bit_check < predecessor->m_bit_check) // has a true child
                {
                    newNode->m_left = newNode;
                    newNode->m_right = predecessor;
                    
                    // rewire
                    if (predecessor->m_right == predecessor)
                        predecessor->m_right = newNode;
                    else if (predecessor->m_left == predecessor)
                        predecessor->m_left = newNode;
                }
                // it's newNode's m_right because its bitTest == 0
                // (so it's right pointer will be pointing to itself)
                else if (predecessor->m_left == predecessor)
                {
                    newNode->m_right = predecessor->m_right;
                    newNode->m_left = newNode;
                    
                    // rewire
                    predecessor->m_right = newNode;
                }
                else if (predecessor->m_right == predecessor)
                {
                    newNode->m_right = predecessor->m_left;
                    newNode->m_left = newNode;
                    
                    // rewire
                    predecessor->m_left = newNode;
                }
            }
            m_node_count++;
            returnedBitCheck = newNode->m_bit_check;
            return true;
        }
    }
    return false;
}

bool Patricia_Tree::lookup(Node *root, char* key)
{
    int prevBitTest = 0;
    int currBitTest = -1;
    
    while (currBitTest < prevBitTest && root != m_dummy)        // traverse the tree (keeping track of bit checks) until
        // the current bit check is greater or equal to the previous
        // bit check AND the root is not at the dummy node
    {
        prevBitTest = root->m_bit_check;                        // keep track of the node predecessor's bit check
        int bitTest = getBitAtPos(key, root->m_bit_check);    // check value's bit at the current Node's bit check position
        if (bitTest == 1) root = root->m_right;                 // if 1, traverse to right child; if 0, traverse to left child
        else root = root->m_left;
        currBitTest = root->m_bit_check;                        // keep track of current node's bit check
    }
    
    int diffBit = getFirstDiffBitPos(key, root->m_value->Key);
    
    if (diffBit == -1) // value found!
        return true;
    
    else return false;
}

bool Patricia_Tree::remove(Node *root, char* key)
{
    int prevBitTest = 0;
    int currBitTest = -1;
    stack<Node *> ptr_stack; // stack to push/pop pointers of visited nodes
    
    while (currBitTest < prevBitTest && root != m_dummy)        // traverse the tree (keeping track of bit checks) until
        // the current bit check is greater or equal to the previous
        // bit check AND the root is not at the dummy node
    {
        ptr_stack.push(root);
        prevBitTest = root->m_bit_check;                        // keep track of the node predecessor's bit check
        int bitTest = getBitAtPos(key, root->m_bit_check);    // check value's bit at the current Node's bit check position
        if (bitTest == 1) root = root->m_right;                 // if 1, traverse to right child; if 0, traverse to left child
        else root = root->m_left;
        currBitTest = root->m_bit_check;                        // keep track of current node's bit check
    }
    
    int diffBit = getFirstDiffBitPos(key, root->m_value->Key); // full key compare.
    
    if (diffBit != -1) return false; // value was not found
    
    else
    {
        if (root == m_root && m_node_count == 1)
        {
            delete m_root;
            m_root = NULL;
            m_node_count--;
            return true;
        }
        
        if (root->m_left == root || root->m_right == root) // node is its own search predecessor
        {
            if (root == m_root) // if deleting the root
            {
                Node *child;
                
                if (root->m_left == root)
                    child = root->m_right;
                
                else if (root->m_right == root)
                    child = root->m_left;
                
                delete root;
                root = NULL;
                m_root = child;
                if (m_node_count == 1)
                {
                    delete m_root;
                    m_root = NULL;
                }
                m_node_count--;
                return true;
            }
            
            else // if deleting a tweener or a leaf
            {
                ptr_stack.pop(); // allow predecessor to be "true" predecessor to root (rather than also pointing to root)
                Node *predecessor = ptr_stack.top();
                ptr_stack.pop();
                
                // rewire
                if (predecessor->m_left == root)
                {
                    if (root->m_left == root)
                        predecessor->m_left = root->m_right;
                    
                    else if (root->m_right == root)
                        predecessor->m_left = root->m_left;
                }
                else if (predecessor->m_right == root)
                {
                    if (root->m_left == root)
                        predecessor->m_right = root->m_right;
                    
                    else if (root->m_right == root)
                        predecessor->m_right = root->m_left;
                }
                
                delete root;
                root = NULL;
                if (m_node_count == 1)
                {
                    delete m_root;
                    m_root = NULL;
                }
                m_node_count--;
                return true;
            }
        }
        
        if (root->m_left != root && root->m_right != root) // node is NOT its own search predecessor
        {
            Node *predecessor = ptr_stack.top();
            ptr_stack.pop();
            
            if ((predecessor->m_left == root && predecessor->m_right == predecessor)
                || (predecessor->m_right == root && predecessor->m_left == predecessor)) // root's predecessor is its own search predecessor
            {
                Node *parentPred = ptr_stack.top();
                ptr_stack.pop();
                
                if (parentPred->m_left == predecessor)
                    parentPred->m_left = root;
                
                else if (parentPred->m_right == predecessor)
                    parentPred->m_right = root;
                
                root->m_value = predecessor->m_value;
                
                delete predecessor;
                predecessor = NULL;
                if (m_node_count == 1)
                {
                    delete m_root;
                    m_root = NULL;
                }
                m_node_count--;
                return true;
            }
            else
            {
                Node *parentPred /*possibly root itself*/ = ptr_stack.top();
                ptr_stack.pop();
                Node *parentPred2 /*possibly m_root*/ = ptr_stack.top();
                ptr_stack.pop();
                
                if (parentPred2->m_right == parentPred)
                {
                    parentPred2->m_right = predecessor;
                }
                else if (parentPred2->m_left == parentPred)
                {
                    parentPred2->m_left = predecessor;
                }
                
                if (parentPred->m_right == predecessor || parentPred->m_left == predecessor)
                {
                    if (predecessor->m_left->m_bit_check >= predecessor->m_bit_check)
                    {
                        predecessor->m_left = predecessor->m_right; // shift
                        predecessor->m_right = parentPred->m_right;
                    }
                    else if (predecessor->m_right->m_bit_check >= predecessor->m_bit_check)
                    {
                        predecessor->m_right = predecessor->m_left; // shift
                        predecessor->m_left = parentPred->m_left;
                    }
                }
                
                delete parentPred;
                parentPred = NULL;
                if (m_node_count == 1)
                {
                    delete m_root;
                    m_root = NULL;
                }
                m_node_count--;
                return true;
            }
        }
    }
    return true;
}

void Patricia_Tree::inOrderPrint()
{
    if (!m_root)
    {
        cout << "Patricia tree is empty" << endl;
    }
    else
    {
        inOrderPrint(m_root, 15);
        cout << endl;
    }
}

void Patricia_Tree::inOrderPrint(Node *root, int previous_bit_check)
{
    if (root->m_bit_check >= previous_bit_check)
    {
        cout << ", " << root->m_value << " " << root->m_bit_check;
        return;
    }
    
    if (root == m_dummy)
    {
        cout << "Dummy";
        return;
    }
    
    inOrderPrint(root->m_left, root->m_bit_check);
    inOrderPrint(root, root->m_bit_check);
    inOrderPrint(root->m_right, root->m_bit_check);
}

