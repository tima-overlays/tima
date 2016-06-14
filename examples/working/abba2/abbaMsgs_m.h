//
// Generated file, do not edit! Created by nedtool 4.6 from inet/applications/abba2/abbaMsgs.msg.
//

#ifndef _INET__ABBA_ABBAMSGS_M_H_
#define _INET__ABBA_ABBAMSGS_M_H_

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

// cplusplus {{
#include "inet/applications/broadcasting/BroadcastingAppBase_m.h"
using inet::broadcasting::Broadcast;
// }}


namespace inet {
namespace abba {

/**
 * Class generated from <tt>inet/applications/abba2/abbaMsgs.msg:25</tt> by nedtool.
 * <pre>
 * packet ABBABroadcast extends inet::broadcasting::Broadcast
 * {
 *     double x;
 *     double y;
 * }
 * </pre>
 */
class INET_API ABBABroadcast : public ::inet::broadcasting::Broadcast
{
  protected:
    double x_var;
    double y_var;

  private:
    void copy(const ABBABroadcast& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ABBABroadcast&);

  public:
    ABBABroadcast(const char *name=NULL, int kind=0);
    ABBABroadcast(const ABBABroadcast& other);
    virtual ~ABBABroadcast();
    ABBABroadcast& operator=(const ABBABroadcast& other);
    virtual ABBABroadcast *dup() const {return new ABBABroadcast(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual double getX() const;
    virtual void setX(double x);
    virtual double getY() const;
    virtual void setY(double y);
};

inline void doPacking(cCommBuffer *b, ABBABroadcast& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ABBABroadcast& obj) {obj.parsimUnpack(b);}

} // namespace abba
} // namespace inet

#endif // ifndef _INET__ABBA_ABBAMSGS_M_H_

