#ifndef PARSER_H
#define PARSER_H

#include <exception>
#include <cstring>
#include <list>
#include <string>

using namespace std;

#define MAX_NAME 80
#define MAX_VALUE 300

class noMoreLinesException : public exception{};
class invalidLineException : public exception{};
class emptyListException : public exception{};
class itemNotFoundException : public exception{};

class line{
    public:
    bool isNode;
    // the actual data fields
    union {
        struct{
            char name[MAX_NAME];
        }node;
        struct{
            char name[MAX_NAME];
            char value[MAX_VALUE];
        }sub;
    }data;
    // member functions
    line(){isNode=false;}
    ~line(){}
    bool operator==(const line& rhs)
    {
        if(isNode!=rhs.isNode)
        {
            return false;
        }
        if(isNode)
        {
            if(strcmpi(data.node.name, rhs.data.node.name))
                return false;
            return true;
        }
        if(!isNode)
        {
            if(strcmpi(data.sub.name, rhs.data.sub.name))
                return false;
            if(strcmpi(data.sub.value, rhs.data.sub.value))
                return false;
            return true;
        }
        return false;
    }
    line& operator=(const line& rhs)
    {
        isNode=rhs.isNode;
        if(isNode)
        {
            strcpy(data.node.name, rhs.data.node.name);
            return *this;
        }else{
            strcpy(data.sub.name, rhs.data.sub.name);
            strcpy(data.sub.value, rhs.data.sub.value);
        }
    }
};

class parser{
    private:
    list<line> dataList;
    string workingNode;
    string getline(string& src, int index) throw(noMoreLinesException);
    bool isws(char val);
    bool isnl(char val);
    public:
    parser()
    {
        workingNode="";
    }
    parser(string initialNode)
    {
        workingNode=initialNode;
    }


    bool load(string& data);
    line parse(string& tline) throw (invalidLineException);
    bool nodeexists(string name);
    bool keyexists(string node, string name);
    bool setworkingnode(string name);
    string getworkingnode();
    bool addnode(string name);
    bool addkey(string node, string name);
    list<line>::iterator getnodeiterator(string name) throw (emptyListException, itemNotFoundException);
    list<line>::iterator getkeyiterator(string node, string key) throw (emptyListException, itemNotFoundException);
    bool killnode(string name);
    bool killnode();
    bool killkey(string node, string key);
    bool killkey(string key);
    bool setvalue(string node, string key, string value);
    string* getvalue(string node, string key);
    float* getvaluefloat(string node, string key);
    int* getvalueint(string node, string key);
    protected:
};

#endif
