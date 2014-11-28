#include <iostream>
#include <cstring>


using namespace std;

typedef unsigned long long u_int64;

enum WarningReasons {
    memoryError,
    fileCreateError,
    fileAccessError,
    writeError,
    readError
};

void PostWarning (const WarningReasons reason) {
    switch (reason) {
        case memoryError:
            printf("ERROR:memmory problem\n");
            break;
        case fileCreateError:
            printf("ERROR:file create problem\n");
            break;
        case fileAccessError:
            printf("ERROR:file access problem\n");
            break;
        case writeError:
            printf("ERROR:cannot write a file\n");
            break;
        case readError:
            printf("ERROR:cannot read a file\n");
            break;
        default:
            break;
    }
}

void* operator new (size_t, const nothrow_t&) throw();
void* operator new[] (size_t, const nothrow_t&) throw();

int min (int a, int b) {
    return a < b ? a : b;
}

int max (int a, int b) {
    return a > b ? a : b;
}

class TVector {
public:
    size_t Capacity;
    size_t Size;
    char* Data;
    size_t GetOptimalCapacity (size_t newCap) {
        size_t res = max(Capacity,1);
        while (res < newCap) {
            res *= 2;
        }
        while ((res/2 > newCap) && (res > 4)) {
            res /= 2;
        }
        return res;
    }

    void Resize (size_t newSize) {
        size_t optimalCap = max(GetOptimalCapacity(newSize),2);
        char* newData = new char[optimalCap];
        if (Data) {
            if (optimalCap > Capacity) {
                for (int i = 0; i < Size; i++) {
                    newData[i] = Data[i];
                }
            }
            else {
                for (int i = 0; i < newSize; i++) {
                    newData[i] = Data[i];
                }
            }
        }
        Size = newSize;
        Capacity = optimalCap;
        if (Data) {
            delete [] Data;
        }
        Data = newData;
    }

    TVector& operator = (TVector& vector) {
        delete[] Data;
        Resize(0);
        Capacity = max(1, vector.Capacity);
        Data = new char[Capacity];
//        if(!Data) {
//            PostWarning(memoryError);
//            exit(0);
//        }
        while (Size < vector.Size) {
            Data[Size] = vector.Data[Size];
            Size++;
        }
        return *this;
    }
    size_t GetSize() {
        return Size;
    }
    bool IsEmpty() {
        return !Size;
    }
    char& operator [] (const int p) {
        return Data[p];
    }
    bool operator == (TVector& vector) {
        if (Size != vector.Size)
            return false;
        for (int i = 0; i < Size; i++) {
            if (Data[i] != vector.Data[i])
                return false;
        }
        return true;
    }
    bool operator != (TVector& vector) {
        return !(*this == vector);
    }
    operator char* () {
        return Data;
    }
    
};


class TString : public TVector {
public:
    virtual ~TString() {};
    TString(char* s);
    TString() {};
    size_t GetSize() {
        return TVector::GetSize()-1;
    }
    char& operator [] (int pos) {
        return TVector::operator[](pos);
    }
    bool operator == (TString& string) {
        return !strcmp(*this, string);
    }
    bool operator != (TString& string) {
        return strcmp(*this, string);
    }
    operator char* () {
        return TVector::operator char*();
    }
};

TString::TString(char *s){
    size_t length = strlen(s);
    Capacity = 1;
    Data = new char[Capacity];
    TVector::Resize(length + 1);
    strcpy((char*)*this, s);
}

int GetBit (TString& string, int n){
    if(n == -1) {
        return 0;
    }
    if (n >= (string.GetSize() << 3)) { //*8
        return 0;
    }
    return string[n >> 3] & (1 << (n & 7)) ? 1 : 0;
}

int DiffBit (TString& str1, TString& str2) {
    size_t minimum = min(str1.GetSize(), str2.GetSize()) + 1;
    for (int i = 0; i < minimum; i++) {
        if(str1[i] != str2[i])
            minimum = i;
    }
    char x = str1[minimum] ^ str2[minimum];
    int ans = minimum << 3;
    if((x & 15) == 0) {
        x >>= 4;
        ans += 4;
    }
    if((x & 3) == 0) {
        x >>= 2;
        ans += 2;
    }
    if((x & 1) == 0) {
        ans += 1;
    }
    return ans;
}


u_int64 Hash (TString& string){
    u_int64 h = string.GetSize();
    char* ptr = (char*)string;
    while(*ptr)
        h = h * 257 + *ptr++;
    return h;
}

u_int64 Hash(u_int64 x) {
    return x;
}


bool FWrite (FILE* file, u_int64 num) {
    return ::fwrite(&num, sizeof(u_int64), 1, file);
}

bool FRead (FILE* file, u_int64& num) {
    return ::fread(&num, sizeof(u_int64), 1, file);
}

bool FWriteString (FILE* file, TString& string){
    size_t size = string.GetSize();
    int a = 0;
    a += ::fwrite(&size, sizeof(int), 1, file);
    a += ::fwrite((char*)string, string.GetSize()*sizeof(char), 1, file);
    return a == 2;
}

bool FReadString(FILE* file, TString& string) {
    int size;
    int a = 0;
    a += ::fread(&size, sizeof(int), 1, file);
    string.Resize(size + 1);
    a += ::fread((char*)string, size * sizeof(char), 1, file);
    string[size] = '\0';
    return a == 2;
}

class TNode {
public:
    TNode* Links[2];
    TString* Key;
    u_int64* Value;
    int NBit;

    TNode() {};
    TNode(TString* key, u_int64* value, int nbit) : Key(key), Value(value), NBit(nbit) {
        Links[0] = this;
        Links[1] = this;
    }
    ~TNode() {
        delete Key;
        delete Value;
        if (Links[0]->NBit > NBit) {
            delete Links[0];
        }
        if (Links[1]->NBit > NBit) {
            delete Links[1];
        }
    }
    int NodeWrite(FILE* file, int id) {
        FWrite(file, id);
        FWrite(file, NBit);
        FWriteString(file, *Key);
        FWrite(file, *Value);
        FWrite(file, Hash(id) + Hash(NBit) + Hash(*Key) + Hash(*Value));
        u_int64* p = Value;
        Value = (u_int64*)((char*)0 + id);
        int cnt = 1;
        if (Links[0]->NBit > NBit) {
            cnt += Links[0]->NodeWrite(file, id + cnt);
        }
        else {
            ::fwrite((int*)&Links[0]->Value, sizeof(int), 1, file);
        }
        if (Links[1]->NBit > NBit) {
            cnt += Links[1]->NodeWrite(file, id + cnt);
        }
        else {
            ::fwrite((int*)&Links[1]->Value, sizeof(int), 1, file);
        }
        Value = p;
        return cnt;
    }
    TNode* NodeRead(FILE* file, int id, int& sum, TNode** arr) {
        u_int64 readId;
        if (!::FRead(file, readId)) {
            return NULL;
        }
        if (readId < id) {
            return arr[readId];
        }
        TNode* t = new TNode;
        arr[id] = t;
        u_int64 nbit = (u_int64)t->NBit;
        if (!FRead(file, nbit)) {
            return NULL;
        }
        t->NBit = nbit;
        t->Key = new TString();
        if (!FReadString(file, *t->Key)) {
            return NULL;
        }
        t->Value = new u_int64;
        if (!FRead(file, *t->Value)) {
            return NULL;
        }
        u_int64 h;
        if (!FRead(file, h)) {
            return NULL;
        }
        if (h != Hash(id) + Hash(t->NBit) + Hash(*t->Key) + Hash(*t->Value)) {
            return NULL;
        }
        int cnt = 1;
        t->Links[0] = NodeRead(file, id + cnt, cnt, arr);
        if (!t->Links[0]) {
            return NULL;
        }
        t->Links[1] = NodeRead(file, id + cnt, cnt, arr);
        if (!t->Links[1]) {
            return NULL;
        }
        
        sum += cnt;
        return t;
    }
};

class TPatricia {
public:
    TNode* Root;
    u_int64 s;
    TNode* Find(TString& key, TNode* start) {
        TNode* t = start;
        int bit = 0;
        if (t != Root) {
            bit = GetBit(key, t->NBit);
        }
        while (t->Links[bit]->NBit > t->NBit) {
            t = t->Links[bit];
            bit = GetBit(key, t->NBit);
        }
        return t;
    }
    TNode* FindDel (TString& key, TNode*& par) {
        TNode* t = Root;
        int bit = 0;
        while (t->Links[bit]->NBit > t->NBit) {
            par = t;
            t = t->Links[bit];
            bit = GetBit(key, t->NBit);
        }
        return t;
    }
    void Insert (TString& key, u_int64 value, int p) {
        TNode* ptr = Root;
        int bit = 0;
        while (ptr->Links[bit]->NBit > ptr->NBit && p >= ptr->Links[bit]->NBit) {
            ptr = ptr->Links[bit];
            bit = GetBit(key, ptr->NBit);
        }
        TNode* t = new TNode (new TString(key), new u_int64(value), p);
        t->Links[GetBit(key, p) ^ 1] = ptr->Links[bit];
        ptr->Links[bit] = t;
    }

    TPatricia() : Root(NULL), s(0) {}
    virtual ~TPatricia() {
        if (s) {
            delete Root;
        }
    }
    int Count (TString& key) {
        if (s) {
            TNode* t = Find(key, Root);
            if (*(t->Links[GetBit(key, t->NBit)]->Key) == key) {
                return 1;
            }
        }
        return 0;
    }
    int Insert (TString& key, u_int64 value) {
        if (!s) {
            Root = new TNode(new TString(key), new u_int64(value), -1);
            s++;
            return 1;
        }
        TNode* t = Find(key, Root);
        if (*(t->Links[GetBit(key, t->NBit)]->Key) == key) {
            return 0;
        }
        int p = DiffBit(key, *(t->Links[GetBit(key, t->NBit)]->Key));
        Insert(key, value, p);
        s++;
        return 1;
    }
    int Delete (TString& key) {
        if (!s) {
            return 0;
        }
        if (s == 1) {
            if (*Root->Key != key) {
                return 0;
            }
            else {
                delete Root;
                Root = NULL;
                s--;
                return 1;
            }
        }
        TNode* par = Root;
        TNode* a = FindDel(key, par);
        TNode* up = a->Links[GetBit(key, a->NBit)];
        if (*up->Key != key) {
            return 0;
        }
        TNode* b = Find(*a->Key, a);
        b->Links[GetBit(*(a->Key), b->NBit)] = up;
        ::swap(up->Key, a->Key);
        ::swap(up->Value, a->Value);
        par->Links[GetBit(key, par->NBit)] = a->Links[GetBit(key, a->NBit)^1];
        a->Links[0] = a->Links[1] = a;
        delete a;
        s--;
        return 1;
    }
    u_int64 GetSize() {
        return s;
    }
    
    u_int64& operator [] (TString& key) {
        if (s) {
            TNode* t = Find(key, Root);
            int pos = GetBit(key, t->NBit);
            if (*(t->Links[pos]->Key) == key) {
                return *(t->Links[pos]->Value);
            }
            else {
                int p = DiffBit(key, *(t->Links[pos]->Key));
                Insert(key, 0, p);
                s++;
                t = Find(key, Root);
                return *(t->Links[GetBit(key, t->NBit)]->Value);
            }
        }
        Root = new TNode(new TString(key), new u_int64(0), -1);
        s++;
        return *(Root->Value);
    }
    void swap(TPatricia& t){
        ::swap(s, t.s);
        ::swap(Root, t.Root);
    }
  
};

void WriteData (FILE* file, TPatricia& patricia) {
    if(!::fwrite("pamaicirtapaekilskoolti", 23, 1, file)) {
        return;
    }
    if (!FWrite(file, patricia.s)) {
        return;
    }
    if (!patricia.s) {
        return;
    }
    patricia.Root->NodeWrite(file, 0);
}
int ReadData (FILE* file, TPatricia& patricia) {
    char arr[23];
    if(!::fread(arr, 23, 1, file)) {
        return 0;
    }
    if(strcmp(arr, "pamaicirtapaekilskoolti")) {
        return -1;
    }
    if (!FRead(file, patricia.s)) {
        return 0;
    }
    if (!patricia.s) {
        return 1;
    }
    TNode** v = new TNode*[patricia.s];
//    if (!v) {
//        PostWarning(memoryError);
//        exit(0);
//    }
    int rs = 0;
    patricia.Root = patricia.Root->NodeRead(file, 0, rs, v);
    delete [] v;
    if (!patricia.Root) {
        return 0;
    }
    return rs == patricia.s;
}

void ToLow (char* c) {
    if (*c >= 'A' && *c <= 'Z') {
        *c += 32;
    }
}

void StringToLow (char* s) {
    u_int64 len = strlen(s);
    for (int i = 0; i < len; i++) {
        ToLow(&s[i]);
    }
}

int main() {
    char str[2049];
    u_int64 num;
    TPatricia patricia;

    while (scanf("%s", str) == 1) {
        //INSERT
        if (*str == '+') {
            scanf("%s %llu", str, &num);
            StringToLow(str);
            if (patricia.Insert(*new TString(str), num)) {
                printf("OK\n");
            }
            else {
                printf("Exist\n");
            }
        }
        //DELETE
        else if (*str == '-') {
            scanf("%s", str);
            StringToLow(str);
            if (patricia.Delete(*new TString(str))) {
                printf("OK\n");
            }
            else {
                printf("NoSuchWord");
            }
        }
        else if (*str == '!') {
            scanf("%s\n", str);
            //SAVE
            if (*str == 'S') {
                scanf("%s", str);
                FILE* f = fopen(str, "wb");
                if (!f) {
                    PostWarning(fileCreateError);
                    continue;
                }
                WriteData(f, patricia);
                printf("OK\n");
                fclose(f);
            }
            //LOAD
            else {
                scanf("%s", str);
                FILE* f = fopen(str, "rb");
                if (!f) {
                    PostWarning(fileAccessError);
                    continue;
                }
                TPatricia tmp;
                int cond = ReadData(f, patricia);
                if (cond == 1) {
                    tmp.swap(patricia);
                    printf("OK\n");
                }
                else if (cond == -1) {
                    printf("ERROR: It is not a patricia file\n");
                }
                else {
                    printf("File is correpted\n");
                }
                fclose(f);
            }
        }
        else {
            StringToLow(str);
            if (!patricia.Count(*new TString(str))) {
                printf("NoSuchWord\n");
            }
            else {
                TString* string = new TString(str);
                printf("OK: %llu\n", patricia[*string]);
            }
        }
    }

    return 0;
}
