// $Id$

#include "Method_Request_Lookup.h"

ACE_RCSID(Notify, TAO_Notify_Method_Request_Lookup, "$Id$")

#include "Consumer_Map.h"
#include "ProxySupplier.h"
#include "ProxyConsumer.h"
#include "Proxy.h"
#include "Admin.h"
#include "SupplierAdmin.h"
#include "Method_Request_Dispatch.h"
#include "Delivery_Request.h"
#include "EventChannelFactory.h"

#include "tao/debug.h"
#include "tao/corba.h"

TAO_Notify_Method_Request_Lookup::TAO_Notify_Method_Request_Lookup (
      const TAO_Notify_Event * event,
      TAO_Notify_ProxyConsumer * proxy)
  : TAO_Notify_Method_Request_Event (event)
  , proxy_consumer_ (proxy)
{
}

TAO_Notify_Method_Request_Lookup::~TAO_Notify_Method_Request_Lookup ()
{
}

void
TAO_Notify_Method_Request_Lookup::work (
  TAO_Notify_ProxySupplier* proxy_supplier
  ACE_ENV_ARG_DECL)
{
  if (delivery_request_.get () == 0)
  {
    TAO_Notify_Method_Request_Dispatch_No_Copy request (*this, proxy_supplier, true);
    proxy_supplier->deliver (request ACE_ENV_ARG_PARAMETER);
  }
  else
  {
    delivery_request_->dispatch (proxy_supplier, true ACE_ENV_ARG_PARAMETER);
    ACE_CHECK;
  }
}

int TAO_Notify_Method_Request_Lookup::execute_i (ACE_ENV_SINGLE_ARG_DECL)
{
  if (this->proxy_consumer_->has_shutdown ())
    return 0; // If we were shutdown while waiting in the queue, return with no action.

  TAO_Notify_SupplierAdmin& parent = this->proxy_consumer_->supplier_admin ();

  CORBA::Boolean val =  this->proxy_consumer_->check_filters (this->event_,
                                                             parent.filter_admin (),
                                                             parent.filter_operator ()
                                                             ACE_ENV_ARG_PARAMETER);

  if (TAO_debug_level > 1)
    ACE_DEBUG ((LM_DEBUG, "Proxyconsumer %x filter eval result = %d",&this->proxy_consumer_ , val));

  // Filter failed - do nothing.
  if (val == 0)
    return 0;

  // The map of subscriptions.
  TAO_Notify_Consumer_Map& map = this->proxy_consumer_->event_manager ().consumer_map ();

  TAO_Notify_Consumer_Map::ENTRY* entry = map.find (this->event_->type () ACE_ENV_ARG_PARAMETER);
  ACE_CHECK_RETURN (0);

  TAO_Notify_ProxySupplier_Collection* consumers = 0;

  if (entry != 0)
  {
    consumers = entry->collection ();

    if (consumers != 0)
      consumers->for_each (this ACE_ENV_ARG_PARAMETER);

    map.release (entry);
  }

  // Get the default consumers
  consumers = map.broadcast_collection ();

  if (consumers != 0)
    consumers->for_each (this ACE_ENV_ARG_PARAMETER);
  this->complete ();
  return 0;
}

/// Static method used to reconstruct a Method Request Dispatch
TAO_Notify_Method_Request_Lookup_Queueable *
TAO_Notify_Method_Request_Lookup::unmarshal (
  TAO_Notify::Delivery_Request_Ptr & delivery_request,
  TAO_Notify_EventChannelFactory &ecf,
  TAO_InputCDR & cdr
  ACE_ENV_ARG_DECL)
{
  bool ok = true;
  TAO_Notify_Method_Request_Lookup_Queueable * result = 0;
  CORBA::ULong count;
  if (cdr.read_ulong (count))
  {
    TAO_Notify::IdVec id_path (count);
    for (size_t nid = 0; ok && nid < count; ++nid)
    {
      TAO_Notify_Object::ID id = 0;
      if ( cdr.read_long (id))
      {
        id_path.push_back (id);
      }
      else
      {
        ok = false;
      }
    }

    if (ok)
    {
      TAO_Notify_ProxyConsumer * proxy_consumer = ecf.find_proxy_consumer (
        id_path,
        0 ACE_ENV_ARG_PARAMETER);
      ACE_CHECK_RETURN(0);
      if (proxy_consumer != 0)
      {
        ACE_NEW_NORETURN (result,
          TAO_Notify_Method_Request_Lookup_Queueable (delivery_request, proxy_consumer));
      }
      else
      {
        ACE_ERROR ((LM_ERROR,
          ACE_TEXT ("(%P|%t) TAO_Notify_Method_Request_Lookup_No_Copy::unmarshal: unknown proxy id\n")
          ));
      }
    }
    else
    {
      ACE_ERROR ((LM_ERROR,
        ACE_TEXT ("(%P|%t) TAO_Notify_Method_Request_Lookup_No_Copy::unmarshal: Cant read proxy id path\n")
        ));
    }
  }
  return result;

}


/****************************************************************/

TAO_Notify_Method_Request_Lookup_Queueable::TAO_Notify_Method_Request_Lookup_Queueable (
      const TAO_Notify_Event::Ptr& event,
      TAO_Notify_ProxyConsumer* proxy_consumer)
  : TAO_Notify_Method_Request_Lookup (event.get (), proxy_consumer)
  , TAO_Notify_Method_Request_Queueable (event.get ())
  , event_var_ (event)
  , proxy_guard_ (proxy_consumer)
{
}

TAO_Notify_Method_Request_Lookup_Queueable::TAO_Notify_Method_Request_Lookup_Queueable (
      TAO_Notify::Delivery_Request_Ptr & request,
      TAO_Notify_ProxyConsumer * proxy_consumer)
  : TAO_Notify_Method_Request_Lookup (request->event ().get (), proxy_consumer)
  , TAO_Notify_Method_Request_Queueable (request->event ().get ())
  , event_var_ (request->event ())
  , proxy_guard_ (proxy_consumer)
{
}


TAO_Notify_Method_Request_Lookup_Queueable::~TAO_Notify_Method_Request_Lookup_Queueable ()
{
}

int
TAO_Notify_Method_Request_Lookup_Queueable::execute (ACE_ENV_SINGLE_ARG_DECL)
{
  return this->execute_i (ACE_ENV_SINGLE_ARG_PARAMETER);
}

/******************************************************************************************************/

TAO_Notify_Method_Request_Lookup_No_Copy::TAO_Notify_Method_Request_Lookup_No_Copy (
        const TAO_Notify_Event* event,
        TAO_Notify_ProxyConsumer* proxy_consumer)
  : TAO_Notify_Method_Request_Lookup (event, proxy_consumer)
{
}

TAO_Notify_Method_Request_Lookup_No_Copy::~TAO_Notify_Method_Request_Lookup_No_Copy ()
{
}

int
TAO_Notify_Method_Request_Lookup_No_Copy::execute (ACE_ENV_SINGLE_ARG_DECL)
{
  return this->execute_i (ACE_ENV_SINGLE_ARG_PARAMETER);
}

TAO_Notify_Method_Request_Queueable*
TAO_Notify_Method_Request_Lookup_No_Copy::copy (ACE_ENV_SINGLE_ARG_DECL)
{
  TAO_Notify_Method_Request_Queueable* request;

  TAO_Notify_Event::Ptr event_var (
    this->event_->queueable_copy (ACE_ENV_SINGLE_ARG_PARAMETER) );
  ACE_CHECK_RETURN (0);

  ACE_NEW_THROW_EX (request,
                    TAO_Notify_Method_Request_Lookup_Queueable (event_var, this->proxy_consumer_),
                    CORBA::INTERNAL ());

  return request;
}
