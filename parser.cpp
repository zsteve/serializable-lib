/** parser.cpp - ini file parser
*	this file is part of the serializable class
*	author : zsteve (stephen zhang)
*/

#include "..\..\macros\macros.h"
#include "parser.h"
#include <exception>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <list>

using namespace std;


// helper functions parser functions

list<line>::iterator parser::getnodeiterator(string name) throw (emptyListException, itemNotFoundException)
{
    /** getnodeiterator()
     * @param name
     * @throw emptyListException, itemNotFoundException
     * @return iterator of the node
     */
    if(dataList.empty())
    {
        throw emptyListException();
    }
    for(list<line>::iterator it=dataList.begin(); it!=dataList.end(); ++it)
    {
        if(it->isNode)
        {
            if(name==it->data.node.name)
            {
                return it;
            }
        }
    }
    throw itemNotFoundException();
}


string parser::getline(string& src, int index) throw(noMoreLinesException)
/** getline()
*   @param data - pointer to the data. index - index of line to retrieve
*   @return line containing the data
*   @throw noMoreLinesException() when there are no more lines
**/
{
    smartAssert(src.length());
    int i, j;
	for(i=0, j=0; i<src.length() && j<index; i++)
	{
		if(src[i]=='\n')
		{
			j++;
			continue;
		}
		if(!src[i])
			throw noMoreLinesException();
	}
	if(!src[i])
            throw noMoreLinesException();
	string cpy;
	for(j=0; (src[i] && (src[i]!='\n')); i++, j++)
		cpy+=src[i];
	cpy+="\0";
	return cpy;
}

// helper function for parser::parse()
bool parser::isws(char val)
{
    /** parser::isws()
    * @param char val - char in question
    * @return returns true for is white space, false for not white space
    */
    if( (val==' ') || (val=='\t'))
    {
        return true;    // ' ' and \t are regarded as white space
    }else{
        return false;
    }
}

// another helper function for parser::parse()
bool parser::isnl(char val)
{
    /** parser::isnl()
    * @param char val - the char in question
    * @return bool, true for newline, else false
    */
    if((val=='\n')||(val=='\r'))
    {
        return true;
    }else{
    return false;
    }
}

line parser::parse(string& tline) throw (invalidLineException)
{
    /** parser::parse()
    * @param string& tline - reference to a string containing the line to parse
    * @return returns a line structure containing information
    * @throw throws invalidLineException if the line is empty or invalid
    */
    line outLine;
    // Step 1 - Skip all leading whitespace
    int i;
    for(i=0; isws(tline[i]); i++)
    {
        if(isnl(tline[i]) || !tline[i]) // premature termination of the line, this is invalid
        {
            throw invalidLineException();
        }
    }
    // Step 2 - We should be up to the name section.
    // now we just have to skip until we either:
    // a. find ws
    // b. reach null terminator
    // c. reach '='
    string name;    // string for name
    for(; (!isws(tline[i]) && (tline[i]) && (tline[i]!='=')); i++)
    {
        name+=tline[i];
    }
    // If we get here, that means that we have finished copying the name
    // and one of the 3 above conditions have been met
    if(name[0]=='[' && name[name.length()-1]==']')
    {
        // if it is a node, first and last chars of the name are '[ and ']
        outLine.isNode=true;
        strcpy(outLine.data.node.name, name.c_str()); // copy the name
        return outLine;
    }
    // otherwise, it is a sub. now we continue skipping ws until the '='
    for(; tline[i]!='='; i++)
    {
        // each time we iterate, we check for nulls and newlines
        if(!tline[i] || isnl(tline[i]))
        {
            throw invalidLineException(); // premature termination of line
        }
    }
    // if we get here, we are ON the "=" sign.
    if(i+1<tline.length())
        i++;    // add one to i to step to next
    else
        throw invalidLineException(); // premature termination of line
    // skip ws
    for(; isws(tline[i]); i++)
    {
        if(isnl(tline[i]) || !tline[i]) // premature termination of the line, this is invalid
        {
            throw invalidLineException();
        }
    }
    string value;   // this is the string for value
    // now we are at the sub's value field
    for(; tline[i] && !isnl(tline[i]); i++) // copy until the end of the line
    {
        value+=tline[i];
    }
    outLine.isNode=false;
    strcpy(outLine.data.sub.name, name.c_str());
    strcpy(outLine.data.sub.value, value.c_str());
    return outLine;
}

bool parser::load(string& data)
{
    /** parser::load() - loads a string& into the parser
     * @param string& data - reference to the string containing the data to parse
     * @throw none
     * @return true for success, false for failure
     */
    int i=0;
    string tempLine;
    line tempParsed;
    while(true)
    {
        try{
            tempLine=getline(data, i);
            tempParsed=parse(tempLine);
        }catch(noMoreLinesException e){
            break;
        }catch(invalidLineException e){
            i++;
            continue;
        }
        dataList.push_back(tempParsed);
        // ** DEBUG **
        // cout << tempParsed.isNode << endl << tempParsed.data.sub.name << endl;
        i++;
    }
    return true;
}

bool parser::nodeexists(string name)
{
    /** parser::nodeexists() - test if a node exists
     * @param string name - name of the node to query
     * @throw none
     * @return true for exists, false for nonexistent
     */
    if(dataList.empty())
        return false;
    for(list<line>::iterator it=dataList.begin(); it!=dataList.end(); ++it)
    {
        if(it->isNode)
        {
            if(name==it->data.node.name)
            {
                return true;
            }
        }
    }
    return false;
}

bool parser::keyexists(string node, string name)
{
    /** parser::keyexists() - tests if a key in given node exists
     * @param : string node, string name
     * @throw : none
     * @return true for exists, false for nonexistent
     */
    list<line>::iterator it;
    if(dataList.empty())
        return false;
    if(!nodeexists(node))
        return false;
    try{
        it=getnodeiterator(node);
    }catch(itemNotFoundException e){
        return false;
    }
    ++it;
    for(; (it!=dataList.end()) && it->isNode==false; ++it)
    {
        if(it->data.sub.name==name)
        {
            return true;
        }
    }
    return false;
}

bool parser::setworkingnode(string name)
{
    /** parser::setworkingnode() - sets the current working node
     * @param name - the name of the node to set as current
     * @throw none
     * @return true for success, false for failure
     */
    if(nodeexists(name))
    {
        workingNode=name;
        return true;
    }
    return false;
}

string parser::getworkingnode()
    /** parser::getworkingnode() - gets the current working node
     * @param none
     * @throw none
     * @return string of the working node
     */
{
    return workingNode;
}

bool parser::addnode(string name)
{
    /** parser::addnode() - appends a node the end of dataList
     * @param name - name of new node
     * @throw none
     * @return true for success, false for failure
     */
    if(nodeexists(name))
    {
        return false;
    }else{
        line newNode;
        newNode.isNode=true;
        strcpy(newNode.data.node.name, name.c_str());
        dataList.push_back(newNode);
        setworkingnode(name);
        return true;
    }
}

bool parser::addkey(string node, string name)
{
    /** parser::addnode() - prepends a key to the given node
     * @param node , name
     * @throw none
     * @return true for success, false for failure
     */
    if(!nodeexists(node))
    {
        return false;
    }
    list<line>::iterator it;
    try{
        it=getnodeiterator(node);
    }catch(itemNotFoundException e)
    {
        return false;
    }
    catch(emptyListException e)
    {
        return false;
    }
    ++it;
    line newItem;
    newItem.isNode=false;
    strcpy(newItem.data.sub.name, name.c_str());
    newItem.data.sub.value[0]=NULL;
    dataList.insert(it, newItem);
    return true;
}

bool parser::killnode(string name)
{
    /** parser::killnode() - kills a node and all of its keys
     * @param name - name of node to kill
     * @throw none
     * @return true for success, false for failure
     */
    if(nodeexists(name))
    {
        list<line>::iterator nodeIt;
        try{
        nodeIt=getnodeiterator(name);
        }catch(emptyListException){
            return false;
        }catch(itemNotFoundException){
            return false;
        }
        // otherwise, the node exists. we will begin deletion
        dataList.erase(nodeIt);
        nodeIt++;
        for(; nodeIt!=dataList.end(); ++nodeIt)
        {
            if(nodeIt->isNode)
            {
                break;
            }
            dataList.erase(nodeIt);
        }
        return true;
    }
    return false;
}

bool parser::killnode()
{
    if(getworkingnode()=="")
        return false;
    bool ret=killnode(getworkingnode());
    workingNode="";
    return ret;
}

bool parser::killkey(string node, string key)
{
    /** parser::killkey() - kills a key
     * @param node - name of node, key - name of key to kill
     * @throw none
     * @return true for success, false for failure
     */
    if(nodeexists(node))
    {
        list<line>::iterator nodeIt;
        try{
            nodeIt=getnodeiterator(node);
        }catch(emptyListException){
            return false;
        }catch(itemNotFoundException){
            return false;
        }
        nodeIt++;
        // otherwise, node exists
        while(!nodeIt->isNode)
        {
            if(nodeIt->data.node.name==key)
            {
                dataList.erase(nodeIt);
                return true;
            }
        }
        return false;
    }
    return false;
}

bool parser::killkey(string key)
{
    return killkey(getworkingnode(), key);
}

list<line>::iterator parser::getkeyiterator(string node, string key) throw (emptyListException, itemNotFoundException)
{
    list<line>::iterator it;
    if(dataList.empty())
        throw emptyListException();
    try{
        it=getnodeiterator(node);
    }catch(itemNotFoundException e){
        throw e;
    }
    // otherwise, we have landed safely and can now search
    ++it;
    for(; it!=dataList.end() && !it->isNode; ++it)
    {
        if(it->data.sub.name==key)
        {
            return it;
        }
    }
    throw itemNotFoundException();
}

bool parser::setvalue(string node, string key, string value)
{
    /** parser::setvalue() - sets given key in given node to given value
     * @param : node, key, value
     * @throw : none
     * @return true for success, false for failure
     */
    list<line>::iterator it;
    if(dataList.empty())
        return false;
    if(!nodeexists(node))
        return false;
    if(!keyexists(node, key))
        return false;
    try{
        it=getkeyiterator(node, key);
    }catch(itemNotFoundException e){
        return false;
    }catch(emptyListException e){
        return false;
    }
    // otherwise, we have the key iterator in it.
    strcpy(it->data.sub.value, value.c_str());
    return true;
}

string* parser::getvalue(string node, string key)
{
    /** parser::getvalue() - gets given key in given node
     * @param : node, key
     * @throw : none
     * @return string* for success, NULL for failure
     */
    list<line>::iterator it;
    if(dataList.empty())
        return NULL;
    if(!nodeexists(node))
        return NULL;
    if(!keyexists(node, key))
        return NULL;
    try{
        it=getkeyiterator(node, key);
    }catch(itemNotFoundException e){
        return NULL;
    }catch(emptyListException e){
        return NULL;
    }
    // otherwise, we have the key iterator in it.
    string* val=new string("");
    *val=it->data.sub.value;
    return val;
}

int* parser::getvalueint(string node, string key)
{
    // wrapper for getvalue() for integers
    string *val;
    if(!(val=getvalue(node, key)))
       return NULL;
    int* v=new int();
    *v=atoi(val->c_str());
    return v;
}

float* parser::getvaluefloat(string node, string key)
{
    // wrapper for getvalue() for floats
    string *val;
    if(!(val=getvalue(node, key)))
       return NULL;
    float* v=new float();
    *v=atof(val->c_str());
    return v;
}
