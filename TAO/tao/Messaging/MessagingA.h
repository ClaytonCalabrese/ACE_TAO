// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from
// be\be_codegen.cpp:754

#ifndef _TAO_IDL_MESSAGINGA_H_
#define _TAO_IDL_MESSAGINGA_H_

#include /**/ "ace/pre.h"

#include "messaging_export.h"
#include "tao/Messaging/MessagingC.h"
#include "tao/Messaging/Messaging_SyncScope_PolicyA.h"
#include "tao/Messaging/Messaging_RT_PolicyA.h"
#include "tao/Messaging/Messaging_No_ImplA.h"
#if defined (TAO_HAS_DEPRECATED_EXCEPTION_HOLDER)
#include "tao/Messaging/OldExceptionHolderA.h"
#else
#include "tao/Messaging/ExceptionHolderA.h"
#endif

// TAO_IDL - Generated from
// be\be_visitor_module/module_ch.cpp:59

namespace Messaging
{

  // TAO_IDL - Generated from
  // be\be_visitor_typecode/typecode_decl.cpp:49

  extern TAO_Messaging_Export ::CORBA::TypeCode_ptr const _tc_ReplyHandler;

// TAO_IDL - Generated from
// be\be_visitor_module/module_ch.cpp:86

} // module Messaging

// TAO_IDL - Generated from
// be\be_visitor_interface/any_op_ch.cpp:54



#if defined (ACE_ANY_OPS_USE_NAMESPACE)

namespace Messaging
{
  TAO_Messaging_Export void operator<<= ( ::CORBA::Any &, ReplyHandler_ptr); // copying
  TAO_Messaging_Export void operator<<= ( ::CORBA::Any &, ReplyHandler_ptr *); // non-copying
  TAO_Messaging_Export ::CORBA::Boolean operator>>= (const ::CORBA::Any &, ReplyHandler_ptr &);
}

#else

TAO_Messaging_Export void operator<<= (::CORBA::Any &, Messaging::ReplyHandler_ptr); // copying
TAO_Messaging_Export void operator<<= (::CORBA::Any &, Messaging::ReplyHandler_ptr *); // non-copying
TAO_Messaging_Export ::CORBA::Boolean operator>>= (const ::CORBA::Any &, Messaging::ReplyHandler_ptr &);

#endif

#include /**/ "ace/post.h"

#endif /* ifndef */
