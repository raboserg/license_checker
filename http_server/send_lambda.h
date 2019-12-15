#ifndef SEND_LAMBDA_H
#define SEND_LAMBDA_H

//#include "session.h"

//  struct send_lambda {
//  session &self_;

//  explicit send_lambda(session &self) : self_(self) {}

//  template <bool isRequest, class Body, class Fields>
//  void operator()(http::message<isRequest, Body, Fields> &&msg) const {
//    // The lifetime of the message has to extend
//    // for the duration of the async operation so
//    // we use a shared_ptr to manage it.
//    auto sp = std::make_shared<http::message<isRequest, Body, Fields>>(
//        std::move(msg));

//    // Store a type-erased version of the shared
//    // pointer in the class to keep it alive.
//    self_.res_ = sp;

//    // Write the response
//    http::async_write(self_.stream_, *sp,
//                      boost::beast::bind_front_handler(&session::on_write,
//                                                       self_.shared_from_this(),
//                                                       sp->need_eof()));
//  }
//};

#endif // SEND_LAMBDA_H
