// $Id$

#include "test_i.h"
#include "ace/Get_Opt.h"
#include "ace/OS_NS_stdio.h"
#include "ace/Argv_Type_Converter.h"

ACE_RCSID (Abstract_Interface,
           server,
           "$Id$")

const char *ior_output_file = "test.ior";

int
parse_args (int argc, char *argv[])
{
  ACE_Argv_Type_Converter convert (argc, argv);
  ACE_Get_Arg_Opt<char> get_opts (convert.get_argc(), convert.get_ASCII_argv(), "o:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
              ior_output_file = get_opts.optarg;
              break;
      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                                             "-o <iorfile>"
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

  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb = CORBA::ORB_init (convert.get_argc(),
                                            convert.get_ASCII_argv(),
                                            ""
                                            ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Create and register factory for BaseNode.
      BaseNode_init *bn_factory = 0;
      ACE_NEW_RETURN (bn_factory,
                      BaseNode_init,
                      1);

      orb->register_value_factory (bn_factory->tao_repository_id (),
                                   bn_factory
                                   ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;
      bn_factory->_remove_ref (); // release ownership

      // Create and register factory for TreeController.
      TreeController_init *tc_factory = 0;
      ACE_NEW_RETURN (tc_factory,
                      TreeController_init,
                      1);

      orb->register_value_factory (tc_factory->tao_repository_id (),
                                   tc_factory
                                   ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;
      tc_factory->_remove_ref (); // release ownership

      // Create and register factory for StringNode.
      StringNode_init *sn_factory = 0;
      ACE_NEW_RETURN (sn_factory,
                      StringNode_init,
                      1);

      orb->register_value_factory (sn_factory->tao_repository_id (),
                                   sn_factory
                                   ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;
      sn_factory->_remove_ref (); // release ownership

      CORBA::Object_var poa_object =
        orb->resolve_initial_references ("RootPOA"
                                         ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (poa_object.in ()))
        {
          ACE_ERROR_RETURN ((LM_ERROR,
                             " (%P|%t) Unable to initialize the POA.\n"),
                            1);
        }

      PortableServer::POA_var root_poa =
        PortableServer::POA::_narrow (poa_object.in ()
                                      ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      PortableServer::POAManager_var poa_manager =
        root_poa->the_POAManager (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      if (parse_args (convert.get_argc(), convert.get_ASCII_argv()) != 0)
        {
          return 1;
        }

      passer_i servant;
      passer_var server =
        servant._this (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      CORBA::String_var ior =
              orb->object_to_string (server.in ()
                                     ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      ACE_DEBUG ((LM_DEBUG,
                  "Activated as <%s>\n",
                  ior.in ()));

      // If the ior_output_file exists, output the ior to it
      if (ior_output_file != 0)
        {
          FILE *output_file = ACE_OS::fopen (ior_output_file, ACE_TEXT("w"));

          if (output_file == 0)
            {
              ACE_ERROR_RETURN ((LM_ERROR,
                                 "Can't open output file for writing IOR: %s",
                                 ior_output_file),
                                1);
            }

          ACE_OS::fprintf (output_file,
                           "%s",
                           ior.in ());
          ACE_OS::fclose (output_file);
        }

      poa_manager->activate (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;

      ACE_Time_Value tv (10);
      orb->run (tv
                ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      // Destroy the POA, waiting until the destruction terminates
      root_poa->destroy (1,
                         1
                         ACE_ENV_ARG_PARAMETER);
      ACE_TRY_CHECK;

      orb->destroy (ACE_ENV_SINGLE_ARG_PARAMETER);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Server: exception caught - ");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
