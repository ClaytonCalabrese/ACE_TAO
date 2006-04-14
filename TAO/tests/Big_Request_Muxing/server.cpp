// $Id$

#include "Payload_Receiver.h"
#include "ace/Get_Opt.h"
#include "ace/OS_NS_stdio.h"
#include "ace/OS_NS_sys_time.h"
#include "ace/Argv_Type_Converter.h"

ACE_RCSID(Big_Request_Muxing, server, "$Id$")

const char *ior_output_file = "test.ior";
static int expected = 600;

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Arg_Opt<char> get_opts (argc, argv, "o:e:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
        ior_output_file = get_opts.opt_arg ();
        break;
      case 'e':
        expected = ACE_OS::atoi(get_opts.opt_arg());
        break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-o <iorfile> [-e <expected>]"
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  ACE_Argv_Type_Converter convert (argc, argv);

  ACE_DEBUG ((LM_DEBUG, "Starting server\n"));

  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (convert.get_argc(), convert.get_ASCII_argv(), "" ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      CORBA::Object_var poa_object =
        orb->resolve_initial_references("RootPOA" ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (poa_object.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                           " (%P|%t) Unable to initialize the POA.\n"),
                          1);

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in () ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (parse_args (convert.get_argc(), convert.get_ASCII_argv()) != 0)
        return 1;

      Payload_Receiver *payload_receiver_impl;
      ACE_NEW_RETURN (payload_receiver_impl,
                      Payload_Receiver(expected),
                      1);
      PortableServer::ServantBase_var receiver_owner_transfer(payload_receiver_impl);

      Test::Payload_Receiver_var payload_receiver =
        payload_receiver_impl->_this (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      CORBA::String_var ior =
        orb->object_to_string (payload_receiver.in () ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // If the ior_output_file exists, output the ior to it
      FILE *output_file= ACE_OS::fopen (ior_output_file, ACE_TEXT("w"));
      if (output_file == 0)
        ACE_ERROR_RETURN ((LM_ERROR,
                           "Cannot open output file for writing IOR: %s",
                           ior_output_file),
                              1);
      ACE_OS::fprintf (output_file, "%s", ior.in ());
      ACE_OS::fclose (output_file);

      poa_manager->activate (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      ACE_DEBUG((LM_DEBUG, "Server waiting for messages...\n"));

      ACE_Time_Value end_time = ACE_OS::gettimeofday() + ACE_Time_Value(25);

      while (payload_receiver_impl->count() < expected)
      {
        ACE_Time_Value tv(0, 100 * 1000);
        orb->run (tv ACE_ENV_ARG_PARAMETER);
        ACE_TRY_CHECK;
        if (ACE_OS::gettimeofday() > end_time)
          break;
      }

      ACE_DEBUG((LM_DEBUG, "Server waiting for extra messages...\n"));

      ACE_Time_Value tv(3);
      orb->run(tv ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      int count = payload_receiver_impl->count();

      int result = 0;

      if (count != expected)
      {
        // Even though 200 events were sent with SYNC_NONE, we still don't
        // expect TAO to drop any events.
        ACE_ERROR((LM_ERROR, "Error: "));
        result = 1;
      }

      ACE_DEBUG ((LM_DEBUG,
                  "(%P) - Server got %d messages, expected %d\n",
                  count,
                  expected));

      root_poa->destroy (1, 1 ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      orb->destroy (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      return result;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "Exception caught:");
    }
  ACE_ENDTRY;

  ACE_DEBUG ((LM_DEBUG, "Ending server\n"));

  return 1;
}
