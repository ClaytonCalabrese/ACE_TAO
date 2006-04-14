// $Id$

#include "Naming_Service.h"
#include "ace/OS_main.h"

#include "orbsvcs/Shutdown_Utilities.h"
#include "tao/debug.h"

class Naming_Svc_Shutdown : public Shutdown_Functor
{
public:
  Naming_Svc_Shutdown(TAO_Naming_Service& ns);

  void operator() (int which_signal);
private:
  TAO_Naming_Service& ns_;
};

Naming_Svc_Shutdown::Naming_Svc_Shutdown (TAO_Naming_Service &ns)
  : ns_(ns)
{
}

void
Naming_Svc_Shutdown::operator() (int which_signal)
{
  if (TAO_debug_level > 0)
    ACE_DEBUG ((LM_DEBUG,
                "Name Service: shutting down on signal %d\n",
                which_signal));
  (void) this->ns_.shutdown ();
}

// Driver function for the TAO Naming Service.

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  TAO_Naming_Service naming_service;

  // Stuff to insure that we're gracefully shut down...
  Naming_Svc_Shutdown killer (naming_service);
  Service_Shutdown kill_contractor(killer);

  if (naming_service.init (argc, argv) == -1)
    ACE_ERROR_RETURN ((LM_ERROR,
                       ACE_TEXT("Failed to start the Naming Service.\n")),
                      1);

  ACE_DECLARE_NEW_CORBA_ENV;
  ACE_TRY
    {
      naming_service.run (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "NamingService");
      return 1;
    }
  ACE_ENDTRY;
  ACE_CHECK_RETURN (1);

  naming_service.fini ();

  return 0;
}
