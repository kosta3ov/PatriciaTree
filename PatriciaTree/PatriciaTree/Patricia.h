#ifndef PATRICIA_TREE_H
#define PATRICIA_TREE_H

#include <iostream>
#include <fstream>
using namespace std;

class TData {
public:
    char* Key;
    int Value;
    TData(char* key, int val) : Key(key), Value(val) {};
    TData();
};

class Patricia_Tree
{
public:
    Patricia_Tree(TData* junk);                        // constructor (ONE argument)
    
    bool insert(TData* data);         // insert function to be called from main
    bool lookup(char* key);         // search function to be called from main
    bool remove(char* key);         // remove/delete function to be called from main
    void inOrderPrint();              // print function to be called from main
    int  getNodeCount() {
        return m_node_count;
    }      // function to determine total nodes in tree
    
private:
    class Node // Patricia Trie's node class
    {
    public:
        Node()          // constructor (FOUR arguments)
        {
            m_left = NULL;
            m_right = NULL;
            m_bit_check = 0;
            m_value = nullptr;
        }
        
        Node *m_left;       // zero_ptr
        Node *m_right;      // one_ptr
        int m_bit_check;    // bit_check
        TData* m_value;        // variable to store integer value
    };
    
    Node *m_root;           // the root pointer to begin with
    Node *m_dummy;          // dummy node
    
    int m_node_count;       // count of total nodes in tree
    
    bool getBitAtPos(char* key, int position);                              // function to determine a 0 or 1 at given bit position
    int  getFirstDiffBitPos(char* key1, char* key2);                        // function to determine first differing MSB
    Node* insert(TData* data, int &returnedBitCheck);                         // first function called by public insert (node count = 0)
    bool insert(Node *root, TData* data, int &returnedBitCheck);              // second function called by public insert (node count > 1)
    bool lookup(Node *root, char* key);                                     // function called by public search
    bool remove(Node *root, char* key);                                     // function called by public remove
    void inOrderPrint(Node *root, int previous_bit_check);  // function called by public inOrderPrint
    void recursiveDelete(Node *root);                                       // function to be called by destructor
};
#endif

