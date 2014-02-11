#include <iostream>
#include "parser.h"
#include "serializable.h"
#include "..\..\macros\macros.h"

using namespace std;

class test : serializable
{
    public:
    test(){}
    ~test(){}
    string serialize()
    {
        string outStr;
        return outStr;
    }
    bool deserialize(char* data)
    {
        return true;
    }
};

test myOb;

parser myParser;
string test="[asdfasdfasfd] = asdasdfasdfasdf\nabc=123\ndef=12.34";

int main()
{
    cout << "unit test for serializable()" << endl;
    myParser.load(test);
    cout << *myParser.getvalue("[asdfasdfasfd]", "abc") << endl;
    cout << *myParser.getvalueint("[asdfasdfasfd]", "abc") << endl;
    cout << (*myParser.getvaluefloat("[asdfasdfasfd]", "def")*3.131) << endl;
    cout << myParser.setvalue("[asdfasdfasfd]", "abc", "this is a new value") << endl;
    cout << *myParser.getvalue("[asdfasdfasfd]", "abc") << endl;
    cout << myParser.nodeexists("[asdfasdfasfd]") << endl;
    cout << myParser.setworkingnode("[asdfasdfasfd]") << endl;
    cout << myParser.getworkingnode() << endl;
    //cout << myParser.killnode("[asdfasdfasfd]") << endl;
    cout << myParser.nodeexists("[asdfasdfasfd]") << endl;
    cout << myParser.keyexists("[asdfasdfasfd]", "abc") << endl;
   // cout << myParser.killkey("[asdfasdfasfd]", "abc") << endl;
    cout << myParser.keyexists("[asdfasdfasfd]", "abc") << endl;
    cout << myParser.setvalue("[asdfasdfasfd]", "abc", "this is a new value") << endl;
    cout << myParser.addkey("[asdfasdfasfd]", "zxcv") << endl;
    cout << myParser.keyexists("[asdfasdfasfd]", "zxcv") << endl;
    cout << myParser.setvalue("[asdfasdfasfd]", "zxcv", "this is a value") << endl;
    cout << *myParser.getvalue("[asdfasdfasfd]", "zxcv") << endl;
    return 0;
}
