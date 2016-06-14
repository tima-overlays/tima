//
// Generated file, do not edit! Created by nedtool 4.6 from inet/applications/tima/Tima.msg.
//

#ifndef _INET_TIMA_M_H_
#define _INET_TIMA_M_H_

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0406
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif

// dll export symbol
#ifndef INET_API
#  if defined(INET_EXPORT)
#    define INET_API  OPP_DLLEXPORT
#  elif defined(INET_IMPORT)
#    define INET_API  OPP_DLLIMPORT
#  else
#    define INET_API
#  endif
#endif


namespace inet {

/**
 * Class generated from <tt>inet/applications/tima/Tima.msg:18</tt> by nedtool.
 * <pre>
 * packet Tima
 * {
 *     string id;
 *     string payload;
 * }
 * </pre>
 */
class INET_API Tima : public ::cPacket
{
  protected:
    opp_string id_var;
    opp_string payload_var;

  private:
    void copy(const Tima& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Tima&);

  public:
    Tima(const char *name=NULL, int kind=0);
    Tima(const Tima& other);
    virtual ~Tima();
    Tima& operator=(const Tima& other);
    virtual Tima *dup() const {return new Tima(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getId() const;
    virtual void setId(const char * id);
    virtual const char * getPayload() const;
    virtual void setPayload(const char * payload);
};

inline void doPacking(cCommBuffer *b, Tima& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Tima& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef _INET_TIMA_M_H_

