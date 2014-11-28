#include <iostream>
#include <fstream>  // for ifstream and ofstream
#include <cctype>   // for toupper
using namespace std;

#include "Patricia.h"

int main(int argc, char *argv[])
{
    
        // instantiate Patricia tree structure
        Patricia_Tree pat_tree(new TData(nullptr, 0));
        char c;
    int value;
    
    char* key;
        while (1)
        {
            c = getchar();
            // if commands are lowercase; turn to uppercase
            // I = Insert value
            // S = Search for value
            // D = Delete value
            // P = Print values in a modified inorder traversal
            
            
            if (c == 'I')
            {

                scanf("%s", key);
                scanf("%d", &value);
                
                TData* newData = new TData();
                newData->Value = value;
                strcpy(key, newData->Key);
                if (pat_tree.insert(newData)) {
                    cout << "OK" << endl;
                }
            }
            
            if (c == 'D')
            {
                scanf("%s", key);
               
                if (pat_tree.remove(key)) {
                    cout << "OK" << endl;
                }
            }
            
            if (c == 'S')
            {
                scanf("%s", key);
                if (pat_tree.lookup(key)) {
                    cout << "OK" << endl;
                }
            }
            
            if (c == 'P')
            {
                pat_tree.inOrderPrint();
            }
        }
    
    return 0;
}