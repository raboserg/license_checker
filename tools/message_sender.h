#pragma once

#include "ace/Connector.h"
#include "ace/SOCK_Connector.h"
#include "ace/Singleton.h"
//#include "ace/Synch.h"
#include "ace/Mutex.h"
#include <cpprest/details/basic_types.h>

// class Client_Svc_Handler
//    : public ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH> {
//  bool debug;
//
// public:
//  Client_Svc_Handler(ACE_Thread_Manager *t = 0);
//  int open(void *v = 0);
//  int close(u_long flags = 0);
//};
//
// Client_Svc_Handler::Client_Svc_Handler(ACE_Thread_Manager *t)
//    : debug(true), ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>(t) {}
//
// int Client_Svc_Handler::open(void *) {
//  if (debug)
//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("opening Client_Svc_Handler %@ with handle %d\n"),
//               this, this->peer().get_handle()));
//  return 0;
//}
//
// int Client_Svc_Handler::close(u_long flags) {
//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("Closing Client_Svc_Handler %@ with handle %d\n"), this,
//             this->peer().get_handle()));
//  return ACE_Svc_Handler<ACE_SOCK_STREAM, ACE_NULL_SYNCH>::close(flags);
//}
//
// typedef ACE_Strategy_Connector<Client_Svc_Handler, ACE_SOCK_CONNECTOR>
//    STRATEGY_CONNECTOR;

class Message_Sender {
  ACE_INET_Addr addr_;

public:
  Message_Sender(const ACE_TCHAR* addr) : addr_(addr) {}

  virtual int open() { return 0; }
  virtual int close() { return 0; }

  virtual int send(const utility::string_t message) {
    ACE_SOCK_Stream stream;
    ACE_SOCK_Connector connect;
    ACE_Time_Value delay(0, 50);
    size_t len = message.size();
    if (connect.connect(stream, addr_) < 0)
      ACE_ERROR_RETURN((LM_ERROR, ACE_TEXT("%T %p \n"),
                        ACE_TEXT("Message_Sernder: faild connect to itVPNAgent (%t): \t ")),
                       -1);
    ACE_DEBUG((LM_DEBUG,
               ACE_TEXT("%T Sending work msg to server on handle 0x%x, req %d :(%t)\n"),
               stream.get_handle()));
    if (stream.send_n(message.c_str(), (len + 1) * sizeof(utility::char_t)) ==
        -1) {
      ACE_ERROR((LM_ERROR, ACE_TEXT("(%t) %p\n"), ACE_TEXT("send_n")));
      ACE_OS::sleep(delay);
    }
    stream.close();
    return 0;
  }
};

//typedef ACE_Singleton<Message_Sender, ACE_Mutex> MESSAGE_SENDER;
