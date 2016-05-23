/*
 * EWMASender.h
 *
 *  Created on: May 23, 2016
 *      Author: inti
 */

#ifndef EWMASENDER_H_
#define EWMASENDER_H_

#include <omnetpp.h>

#include <string>
#include <set>


class EWMASender : public cObject {
public:
    EWMASender();
    virtual ~EWMASender();

    void setSender(const std::string& s);
    std::string getSender();

    void add(const std::string& e);
    bool isMember(const std::string& e);
private:
    std::string sender;
    std::set<std::string> v_p;
};


void doPacking(cCommBuffer *, EWMASender& t);

void doUnpacking(cCommBuffer *, EWMASender& t);


#endif /* EWMASENDER_H_ */
