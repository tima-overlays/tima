# Introduction

# Development

There are three directories:
 - **tima-compiler:** A Maven Project that contains the implementation of the intermediate representations used during the compilation of an automaton. There are actually two representations, a non-compiled one and a compiled (or simplified) one.

 - **tima-dsl:** A XText-based language to write gossip/netowrk protocols. It generates the source code (in *C/C++*) of native *Linux* applications as well as OmNet++/Inet applications in order to simulate the protocols. This directory is just an Eclipse workspace.  

 - **native-runtime:** It contains the source code for the runtime support of the language. The concepts used in the language are too abstract. As a consequence, we need to represent them using a runtime environment. It contains the data structures and algorithms to execute an automatons and in general network protocols.

# Language Description

Take a look at the [full grammar](tima-dsl/fr.labri.gossip.tima/src/fr/labri/gossip/tima/DSL.xtext) of the language.

## States

A simple example where there is a compound action. The first simple action is the execution of a method **method1** defined elsewhere. The second simple action send a message **a** to **automaton2**.

    state s0
      action
        :: Class1::method1
        ! a : automaton2

      on :: Class2::guard1 within 100 msec => s2
      on timeout => s1

Notice that there are two possible transitions. First, we can go to state **s2**
if **guard1** is true in the first 100 msec after the s0 becomes the current state. If this doesn't happen, we take the *default* transition to **s1**.

## Messages

You have to declare the messages used in the protocol. These messages are used to perform communication among automata in the same device and also network communication. So far, we are only interested in their name -- this will change really soon. The declaration of messages is done as follow:

```
messages msg1, msg2, msg3
```

## Guards

There are three type of guards.

**Arbitrary Guard**

Arbitrary guards are implemented as *static* methods of *C++* classes.

```
:: Classname::Methodname
```

and the implementation of this guard can be something like:

```C++
bool Classname::Methodname(std::string& name, tima::TimaNativeContext* context)
{
  bool b;
  // code to compute b
  return b;
}
```

**Local message from a different automaton**

```
? message_id : automaton_id
```

**Remote message from the network**

```
>> message_id
```

## Automaton

# Example (It just shows the grammar and how to define arbitrary actions)

```
name ex1

messages a z

automaton Main
	initial state s0 		
		on ? a : Secondary within 500 msec => s1
		on :: C::b within 1000 msec => s2
		on >> z within 700 msec => s5
		on timeout => s3

	urgent state s1
		action
			:: D::x

		on ::C::True => s0

	state s2
		on ::C::c within 400 msec => s1
		on timeout => s3

	state s3
		on :: C::d within 1500 msec => s4
		on timeout => s0


	urgent state s4
		on ::C::True => s0

	urgent state s5
		action
			:: D::y

		on ::C::True => s0

/**
 * This is a ticker
 */
automaton Secondary
	initial state s0
		on :: C::False within 700 msec => s2
		on timeout => s1

	urgent state s1
		action
			! a : Main

		on :: C::True => s0

	state s2
		on :: C::False => s0
```

The code associated to each arbitrary guard and action is listed below:

```C++
#include "ex1.h"
#include "mailbox.hpp"
#include "tima_utils.h"

#include <iostream>

// bool C::a(std::string& name, TimaNativeContext* context)
// {
//   return true;
// }

bool C::b(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::c(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::d(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

bool C::True(std::string& name, tima::TimaNativeContext* context)
{
  return true;
}

bool C::False(std::string& name, tima::TimaNativeContext* context)
{
  return false;
}

void D::x(std::string& name, tima::TimaNativeContext* context)
{
  std::cout << "Entering the state because I received a tick" << std::endl;
}

void D::y(std::string& name, tima::TimaNativeContext* context)
{
  auto ctx = (tima::GenericActionContext*)context;
  if (ctx->msg_received) {
    std::cout << "Entering the state because I received a MESSAGE : " << ctx->msg.msg_id << " with payload: "  << std::endl << "\t";
    for (auto it = ctx->msg.payload.begin(), itEnd = ctx->msg.payload.end(); it != itEnd ; ++it) {
      std::cout << *it;
    }
    std::cout << std::endl;
  }
}
```

# Usage