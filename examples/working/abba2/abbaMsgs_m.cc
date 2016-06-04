//
// Generated file, do not edit! Created by nedtool 4.6 from inet/applications/abba2/abbaMsgs.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "abbaMsgs_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}



namespace inet {
namespace abba {

// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

Register_Class(ABBABroadcast);

ABBABroadcast::ABBABroadcast(const char *name, int kind) : ::inet::broadcasting::Broadcast(name,kind)
{
    this->x_var = 0;
    this->y_var = 0;
}

ABBABroadcast::ABBABroadcast(const ABBABroadcast& other) : ::inet::broadcasting::Broadcast(other)
{
    copy(other);
}

ABBABroadcast::~ABBABroadcast()
{
}

ABBABroadcast& ABBABroadcast::operator=(const ABBABroadcast& other)
{
    if (this==&other) return *this;
    ::inet::broadcasting::Broadcast::operator=(other);
    copy(other);
    return *this;
}

void ABBABroadcast::copy(const ABBABroadcast& other)
{
    this->x_var = other.x_var;
    this->y_var = other.y_var;
}

void ABBABroadcast::parsimPack(cCommBuffer *b)
{
    ::inet::broadcasting::Broadcast::parsimPack(b);
    doPacking(b,this->x_var);
    doPacking(b,this->y_var);
}

void ABBABroadcast::parsimUnpack(cCommBuffer *b)
{
    ::inet::broadcasting::Broadcast::parsimUnpack(b);
    doUnpacking(b,this->x_var);
    doUnpacking(b,this->y_var);
}

double ABBABroadcast::getX() const
{
    return x_var;
}

void ABBABroadcast::setX(double x)
{
    this->x_var = x;
}

double ABBABroadcast::getY() const
{
    return y_var;
}

void ABBABroadcast::setY(double y)
{
    this->y_var = y;
}

class ABBABroadcastDescriptor : public cClassDescriptor
{
  public:
    ABBABroadcastDescriptor();
    virtual ~ABBABroadcastDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ABBABroadcastDescriptor);

ABBABroadcastDescriptor::ABBABroadcastDescriptor() : cClassDescriptor("inet::abba::ABBABroadcast", "inet::broadcasting::Broadcast")
{
}

ABBABroadcastDescriptor::~ABBABroadcastDescriptor()
{
}

bool ABBABroadcastDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ABBABroadcast *>(obj)!=NULL;
}

const char *ABBABroadcastDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ABBABroadcastDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 2+basedesc->getFieldCount(object) : 2;
}

unsigned int ABBABroadcastDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<2) ? fieldTypeFlags[field] : 0;
}

const char *ABBABroadcastDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "x",
        "y",
    };
    return (field>=0 && field<2) ? fieldNames[field] : NULL;
}

int ABBABroadcastDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='x' && strcmp(fieldName, "x")==0) return base+0;
    if (fieldName[0]=='y' && strcmp(fieldName, "y")==0) return base+1;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ABBABroadcastDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "double",
        "double",
    };
    return (field>=0 && field<2) ? fieldTypeStrings[field] : NULL;
}

const char *ABBABroadcastDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ABBABroadcastDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ABBABroadcast *pp = (ABBABroadcast *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ABBABroadcastDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ABBABroadcast *pp = (ABBABroadcast *)object; (void)pp;
    switch (field) {
        case 0: return double2string(pp->getX());
        case 1: return double2string(pp->getY());
        default: return "";
    }
}

bool ABBABroadcastDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ABBABroadcast *pp = (ABBABroadcast *)object; (void)pp;
    switch (field) {
        case 0: pp->setX(string2double(value)); return true;
        case 1: pp->setY(string2double(value)); return true;
        default: return false;
    }
}

const char *ABBABroadcastDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *ABBABroadcastDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ABBABroadcast *pp = (ABBABroadcast *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

} // namespace abba
} // namespace inet

