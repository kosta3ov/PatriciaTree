//
//  Patricia.cpp
//  PatriciaTree
//
//  Created by Константин on 02.11.14.
//  Copyright (c) 2014 KONSTANTINPRODUCTIONS. All rights reserved.
//

#include "Patricia.h"

TPatriciaTree::TPatriciaTree(TPair* newData) {
    Root = nullptr;
    Dummy = new TNode ();
    Dummy->Data = newData;
    NodeCount = 0;
}

bool TPatriciaTree::GetBitAtPos(char* key, int position) {
    //MAXPOSITION 0 ... 2047
    return *(key + position/8) & (1 << (position % 8));
}

int TPatriciaTree::GetFirstDiffBitPos(char* key1, char* key2) {
    int position =  0;
    
    while ((GetBitAtPos(key1, position) == GetBitAtPos(key2, position)) && position >= 2047)
    {
        position++;
    }
    
    return position; //return diff bit position
}

bool TPatriciaTree::Insert (TPair* newData) {
    bool insertion = false;
    int returnedBitCheck = 0;
    
    if (!Root) {
        Root = Insert(newData, returnedBitCheck);
        insertion = true;
    }
    else {
        insertion = Insert(Root, newData, returnedBitCheck);
    }
    if (insertion == true) {
        cout << "Successful insertion with node bit test " << returnedBitCheck << endl;
        return true;
    }
    else {
        cout << "Illegal attempt to insert duplicate value" << endl;
        return false;
    }
}

bool TPatriciaTree::LookFor(char* key) {
    bool search = false;
    if (Root) {
        search = LookFor(Root, key);
    }
    if (search == true) {
        cout << "Value found! " << endl;
        return true;
    }
    else {
        cout << "Value not found! " << endl;
        return false;
    }
}

bool TPatriciaTree::Remove(char* key) {
    bool removed = false;
    if (Root) {
        removed = Remove(Root, key);
    }
    if (removed == true) {
        cout << "Successful deletion! " << endl;
        return true;
    }
    else {
        cout << "Illegal attempt to delete a value not present! " << endl;
        return false;
    }
}


TNode* TPatriciaTree::Insert(TPair* newData, int &returnedBitCheck) {
    Root = new TNode();
    int diffBit = GetFirstDiffBitPos(newData->Key->KeyVal, Dummy->Data->Key->KeyVal);
    Root->BitCheck = diffBit;
    Root->Data = new TPair();
    Root->Data->Value = newData->Value;
    Root->Data->Key = new TKey();
    Root->Data->Key->KeyVal = newData->Key->KeyVal;
    Root->Left = Dummy;
    Root->Right = Root;
    NodeCount++;
    returnedBitCheck = Root->BitCheck;
    return Root;
}

bool TPatriciaTree::Insert(TNode* root, TPair* newData, int&returnedBitCheck) {
    int prevBitTest = 0;
    int curBitTest = -1;
    
    
}






