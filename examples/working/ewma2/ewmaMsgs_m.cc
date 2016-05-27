//
// Generated file, do not edit! Created by nedtool 4.6 from inet/applications/ewma2/ewmaMsgs.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ewmaMsgs_m.h"

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
namespace ewma {

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

Register_Class(EWMABroadcast);

EWMABroadcast::EWMABroadcast(const char *name, int kind) : ::inet::broadcasting::Broadcast(name,kind)
{
    covered_arraysize = 0;
    this->covered_var = 0;
}

EWMABroadcast::EWMABroadcast(const EWMABroadcast& other) : ::inet::broadcasting::Broadcast(other)
{
    covered_arraysize = 0;
    this->covered_var = 0;
    copy(other);
}

EWMABroadcast::~EWMABroadcast()
{
    delete [] covered_var;
}

EWMABroadcast& EWMABroadcast::operator=(const EWMABroadcast& other)
{
    if (this==&other) return *this;
    ::inet::broadcasting::Broadcast::operator=(other);
    copy(other);
    return *this;
}

void EWMABroadcast::copy(const EWMABroadcast& other)
{
    delete [] this->covered_var;
    this->covered_var = (other.covered_arraysize==0) ? NULL : new opp_string[other.covered_arraysize];
    covered_arraysize = other.covered_arraysize;
    for (unsigned int i=0; i<covered_arraysize; i++)
        this->covered_var[i] = other.covered_var[i];
}

void EWMABroadcast::parsimPack(cCommBuffer *b)
{
    ::inet::broadcasting::Broadcast::parsimPack(b);
    b->pack(covered_arraysize);
    doPacking(b,this->covered_var,covered_arraysize);
}

void EWMABroadcast::parsimUnpack(cCommBuffer *b)
{
    ::inet::broadcasting::Broadcast::parsimUnpack(b);
    delete [] this->covered_var;
    b->unpack(covered_arraysize);
    if (covered_arraysize==0) {
        this->covered_var = 0;
    } else {
        this->covered_var = new opp_string[covered_arraysize];
        doUnpacking(b,this->covered_var,covered_arraysize);
    }
}

void EWMABroadcast::setCoveredArraySize(unsigned int size)
{
    opp_string *covered_var2 = (size==0) ? NULL : new opp_string[size];
    unsigned int sz = covered_arraysize < size ? covered_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        covered_var2[i] = this->covered_var[i];
    for (unsigned int i=sz; i<size; i++)
        covered_var2[i] = 0;
    covered_arraysize = size;
    delete [] this->covered_var;
    this->covered_var = covered_var2;
}

unsigned int EWMABroadcast::getCoveredArraySize() const
{
    return covered_arraysize;
}

const char * EWMABroadcast::getCovered(unsigned int k) const
{
    if (k>=covered_arraysize) throw cRuntimeError("Array of size %d indexed by %d", covered_arraysize, k);
    return covered_var[k].c_str();
}

void EWMABroadcast::setCovered(unsigned int k, const char * covered)
{
    if (k>=covered_arraysize) throw cRuntimeError("Array of size %d indexed by %d", covered_arraysize, k);
    this->covered_var[k] = covered;
}

class EWMABroadcastDescriptor : public cClassDescriptor
{
  public:
    EWMABroadcastDescriptor();
    virtual ~EWMABroadcastDescriptor();

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

Register_ClassDescriptor(EWMABroadcastDescriptor);

EWMABroadcastDescriptor::EWMABroadcastDescriptor() : cClassDescriptor("inet::ewma::EWMABroadcast", "inet::broadcasting::Broadcast")
{
}

EWMABroadcastDescriptor::~EWMABroadcastDescriptor()
{
}

bool EWMABroadcastDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<EWMABroadcast *>(obj)!=NULL;
}

const char *EWMABroadcastDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int EWMABroadcastDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 1+basedesc->getFieldCount(object) : 1;
}

unsigned int EWMABroadcastDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<1) ? fieldTypeFlags[field] : 0;
}

const char *EWMABroadcastDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "covered",
    };
    return (field>=0 && field<1) ? fieldNames[field] : NULL;
}

int EWMABroadcastDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='c' && strcmp(fieldName, "covered")==0) return base+0;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *EWMABroadcastDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "string",
    };
    return (field>=0 && field<1) ? fieldTypeStrings[field] : NULL;
}

const char *EWMABroadcastDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
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

int EWMABroadcastDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    EWMABroadcast *pp = (EWMABroadcast *)object; (void)pp;
    switch (field) {
        case 0: return pp->getCoveredArraySize();
        default: return 0;
    }
}

std::string EWMABroadcastDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    EWMABroadcast *pp = (EWMABroadcast *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getCovered(i));
        default: return "";
    }
}

bool EWMABroadcastDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    EWMABroadcast *pp = (EWMABroadcast *)object; (void)pp;
    switch (field) {
        case 0: pp->setCovered(i,(value)); return true;
        default: return false;
    }
}

const char *EWMABroadcastDescriptor::getFieldStructName(void *object, int field) const
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

void *EWMABroadcastDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    EWMABroadcast *pp = (EWMABroadcast *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

} // namespace ewma
} // namespace inet

