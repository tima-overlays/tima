/*
 * EWMASender.h
 *
 *  Created on: May 23, 2016
 *      Author: inti
 */

#ifndef EWMASENDER_H_
#define EWMASENDER_H_

#include <string>
#include <set>

namespace inet {

class EWMASender {
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

} /* namespace inet */

#endif /* EWMASENDER_H_ */
