/*
 * EWMASender.h
 *
 *  Created on: May 23, 2016
 *      Author: inti
 */

#ifndef EWMASENDER_H_
#define EWMASENDER_H_

#include <string>

namespace inet {

class EWMASender {
public:
    EWMASender();
    virtual ~EWMASender();
private:
    std::string sender;
};

} /* namespace inet */

#endif /* EWMASENDER_H_ */
