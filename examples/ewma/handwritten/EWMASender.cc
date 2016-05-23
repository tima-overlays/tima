/*
 * EWMASender.cpp
 *
 *  Created on: May 23, 2016
 *      Author: inti
 */

#include "EWMASender.h"

namespace inet {

EWMASender::EWMASender() {
}

EWMASender::~EWMASender() {
}



void
EWMASender::setSender(const std::string& s)
{
    sender = s;
}


std::string
EWMASender::getSender()
{
    return sender;
}


void
EWMASender::add(const std::string& e)
{
    v_p.insert(e);
}


bool
EWMASender::isMember(const std::string& e)
{
    return v_p.find(e) != v_p.end();
}

} /* namespace inet */
