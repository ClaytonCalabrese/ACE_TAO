//$Id$
#include "Manager_i.h"

#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include "ace/Argv_Type_Converter.h"

int ACE_TMAIN(int argc, ACE_TCHAR* argv[])
{
  ACE_Argv_Type_Converter convert (argc, argv);

  try
  {
    long retryCount = 0;
    CORBA::ORB_var orb = CORBA::ORB_init(convert.get_argc(), convert.get_ASCII_argv());

    ACE_Get_Arg_Opt<char> get_opts(convert.get_argc(), convert.get_ASCII_argv(), "r:");
    int c;
    while ((c = get_opts()) != -1)
    {
      switch (c)
      {
      case 'r':
        retryCount = ::atoi(get_opts.opt_arg());
        break;
      case '?':
      default:
        cout << "Usage:  " << argv[0] << " [-r retryCount]" << endl;
        break;
      }
    }

    CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(obj.in());

    PortableServer::POAManager_var mgr = poa->the_POAManager();

    Manager_i servant(retryCount);
    PortableServer::ObjectId_var oid = poa->activate_object(&servant);
    obj = poa->id_to_reference(oid.in());
    {
      CORBA::String_var ior = orb->object_to_string(obj.in());
      ofstream iorFile("manager.ior");
      iorFile << ior.in() << endl;
    }

    cout << "IMR Test Manager activated (servers retry = " << retryCount << ")." << endl;

    mgr->activate();

    orb->run();

    poa->destroy(1, 1);
    orb->destroy();
    return 0;
  }
  catch (CORBA::Exception& ex)
  {
    cerr << "Manager: " << ex << endl;
  }
  return 1;
}
