//
//  Patricia.h
//  PatriciaTree
//
//  Created by Константин on 02.11.14.
//  Copyright (c) 2014 KONSTANTINPRODUCTIONS. All rights reserved.
//

#ifndef __PatriciaTree__Patricia__
#define __PatriciaTree__Patricia__

#include <iostream>

using namespace std;

class TKey {
public:
    char* KeyVal;
  //  int KeyLength; optional
};

class TPair {
public:
    int Value;
    TKey* Key;
};

class TNode {
    
public:
    TNode();
    TNode* Left;
    TNode* Right;
    int BitCheck;
    TPair* Data;
};

class TPatriciaTree {
    
public:
    // functions called from main
    TPatriciaTree (TPair* junk);
    bool Insert (TPair* newData);
    bool LookFor (char* key);
    bool Remove (char* key);
    void Print ();
    int GetNodeCount ();
    
private:
    //same functions called by public functions
    TNode* Root;
    TNode* Dummy;
    int NodeCount;
    
    bool GetBitAtPos (char* key, int position);
    int GetFirstDiffBitPos (char* key1, char* key2);
    TNode* Insert (TPair* newData, int &returnedBitCheck); // first insert (node count = 0)
    bool Insert (TNode* root, TPair* newData, int &returnedBitCheck); // node count > 1
    bool LookFor (TNode* root, char* key);
    bool Remove (TNode* root, char* key);
    void Print (TNode* root, int previous_bit_check);
    void RecursiveDelete(TNode* root);  // function to be called by destructor

};

#endif /* defined(__PatriciaTree__Patricia__) */
