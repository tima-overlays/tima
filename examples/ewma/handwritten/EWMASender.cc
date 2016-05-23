/*
 * EWMASender.cpp
 *
 *  Created on: May 23, 2016
 *      Author: inti
 */

#include "EWMASender.h"

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


void doPacking(cCommBuffer *, EWMASender& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

void doUnpacking(cCommBuffer *, EWMASender& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

