//
// Generated file, do not edit! Created by nedtool 4.6 from inet/applications/mwst2/MWSTMsgs.msg.
//

#ifndef _INET_MWSTMSGS_M_H_
#define _INET_MWSTMSGS_M_H_

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
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:18</tt> by nedtool.
 * <pre>
 * packet Hello
 * {
 *     string sender;
 *     int x;
 *     int y;
 * }
 * </pre>
 */
class INET_API Hello : public ::cPacket
{
  protected:
    opp_string sender_var;
    int x_var;
    int y_var;

  private:
    void copy(const Hello& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Hello&);

  public:
    Hello(const char *name=NULL, int kind=0);
    Hello(const Hello& other);
    virtual ~Hello();
    Hello& operator=(const Hello& other);
    virtual Hello *dup() const {return new Hello(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
    virtual int getX() const;
    virtual void setX(int x);
    virtual int getY() const;
    virtual void setY(int y);
};

inline void doPacking(cCommBuffer *b, Hello& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Hello& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:24</tt> by nedtool.
 * <pre>
 * packet Connect
 * {
 *     string sender;
 * }
 * </pre>
 */
class INET_API Connect : public ::cPacket
{
  protected:
    opp_string sender_var;

  private:
    void copy(const Connect& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Connect&);

  public:
    Connect(const char *name=NULL, int kind=0);
    Connect(const Connect& other);
    virtual ~Connect();
    Connect& operator=(const Connect& other);
    virtual Connect *dup() const {return new Connect(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Connect& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Connect& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:28</tt> by nedtool.
 * <pre>
 * packet Initiate
 * {
 *     string fragmentId;
 *     string sender;
 * }
 * </pre>
 */
class INET_API Initiate : public ::cPacket
{
  protected:
    opp_string fragmentId_var;
    opp_string sender_var;

  private:
    void copy(const Initiate& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Initiate&);

  public:
    Initiate(const char *name=NULL, int kind=0);
    Initiate(const Initiate& other);
    virtual ~Initiate();
    Initiate& operator=(const Initiate& other);
    virtual Initiate *dup() const {return new Initiate(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getFragmentId() const;
    virtual void setFragmentId(const char * fragmentId);
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Initiate& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Initiate& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:34</tt> by nedtool.
 * <pre>
 * packet Test
 * {
 *     string fragmentId;
 *     string sender;
 * }
 * </pre>
 */
class INET_API Test : public ::cPacket
{
  protected:
    opp_string fragmentId_var;
    opp_string sender_var;

  private:
    void copy(const Test& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Test&);

  public:
    Test(const char *name=NULL, int kind=0);
    Test(const Test& other);
    virtual ~Test();
    Test& operator=(const Test& other);
    virtual Test *dup() const {return new Test(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getFragmentId() const;
    virtual void setFragmentId(const char * fragmentId);
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Test& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Test& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:39</tt> by nedtool.
 * <pre>
 * packet Accept
 * {
 *     string sender;
 * }
 * </pre>
 */
class INET_API Accept : public ::cPacket
{
  protected:
    opp_string sender_var;

  private:
    void copy(const Accept& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Accept&);

  public:
    Accept(const char *name=NULL, int kind=0);
    Accept(const Accept& other);
    virtual ~Accept();
    Accept& operator=(const Accept& other);
    virtual Accept *dup() const {return new Accept(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Accept& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Accept& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:43</tt> by nedtool.
 * <pre>
 * packet Reject
 * {
 *     string sender;
 * }
 * </pre>
 */
class INET_API Reject : public ::cPacket
{
  protected:
    opp_string sender_var;

  private:
    void copy(const Reject& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Reject&);

  public:
    Reject(const char *name=NULL, int kind=0);
    Reject(const Reject& other);
    virtual ~Reject();
    Reject& operator=(const Reject& other);
    virtual Reject *dup() const {return new Reject(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Reject& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Reject& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:47</tt> by nedtool.
 * <pre>
 * packet Report
 * {
 *     int weight;
 *     string sender;
 * }
 * </pre>
 */
class INET_API Report : public ::cPacket
{
  protected:
    int weight_var;
    opp_string sender_var;

  private:
    void copy(const Report& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Report&);

  public:
    Report(const char *name=NULL, int kind=0);
    Report(const Report& other);
    virtual ~Report();
    Report& operator=(const Report& other);
    virtual Report *dup() const {return new Report(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual int getWeight() const;
    virtual void setWeight(int weight);
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, Report& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, Report& obj) {obj.parsimUnpack(b);}

/**
 * Class generated from <tt>inet/applications/mwst2/MWSTMsgs.msg:52</tt> by nedtool.
 * <pre>
 * packet ChangeRoot
 * {
 *     string sender;
 * }
 * </pre>
 */
class INET_API ChangeRoot : public ::cPacket
{
  protected:
    opp_string sender_var;

  private:
    void copy(const ChangeRoot& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ChangeRoot&);

  public:
    ChangeRoot(const char *name=NULL, int kind=0);
    ChangeRoot(const ChangeRoot& other);
    virtual ~ChangeRoot();
    ChangeRoot& operator=(const ChangeRoot& other);
    virtual ChangeRoot *dup() const {return new ChangeRoot(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual const char * getSender() const;
    virtual void setSender(const char * sender);
};

inline void doPacking(cCommBuffer *b, ChangeRoot& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ChangeRoot& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef _INET_MWSTMSGS_M_H_
