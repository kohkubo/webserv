#ifndef SRCS_EVENT_TRANSACTION
#define SRCS_EVENT_TRANSACTION

#include "event/Response.hpp"
#include "http/request/RequestInfo.hpp"

class Transaction {
public:
  RequestInfo request_info_;
  Response    response_;

  Transaction(const RequestInfo &request_info)
      : request_info_(request_info) {}
};

#endif /* SRCS_EVENT_TRANSACTION */
