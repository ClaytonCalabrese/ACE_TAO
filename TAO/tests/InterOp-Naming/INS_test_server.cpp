// $Id$
#include "Server_i.h"
#include "INS_i.h"

// This is the main driver program for the INS test server.

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  ACE_Argv_Type_Converter convert (argc, argv);

  Server_i server;

  ACE_DEBUG ((LM_DEBUG,
              "\n\tINS test server\n\n"));

  ACE_TRY_NEW_ENV
    {
      if (server.init (convert.get_argc(), convert.get_ASCII_argv()
                       ACE_ENV_ARG_PARAMETER) == -1)
        return 1;
      else
        {
          server.run (ACE_ENV_SINGLE_ARG_PARAMETER);
          ACE_TRY_CHECK;
        }
    }
  ACE_CATCH (CORBA::SystemException, sysex)
    {
      ACE_PRINT_EXCEPTION (sysex, "System Exception");
      return -1;
    }
  ACE_CATCH (CORBA::UserException, userex)
    {
      ACE_PRINT_EXCEPTION (userex, "User Exception");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}
