#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H
#include "parser.h"

class serializable{
    public:
    virtual ~serializable()=0;
    virtual string serialize()=0;
    virtual bool deserialize(char* data)=0;
    protected:
    parser myParser;
};

serializable::~serializable(){}

#endif
