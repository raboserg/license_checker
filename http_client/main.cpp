#include "http_client.h"


int ACE_TMAIN(int, ACE_TCHAR *[]) {
  // ACE_INET_Addr port_to_connect(ACE_TEXT("HAStatus"), ACE_LOCALHOST);
  ACE_INET_Addr port_to_connect("localhost", 9090);
  
  ACE_OS::printf(port_to_connect.get_host_addr());
  
  ACE_Connector<Client, ACE_SOCK_CONNECTOR> connector;
  Client client;
  Client *pc = &client;
  if (connector.connect(pc, port_to_connect) == -1)
    ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("connect")), 1);

  ACE_Reactor::instance()->run_reactor_event_loop();
  return (0);
}