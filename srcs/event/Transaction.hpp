#ifndef SRCS_EVENT_TRANSACTION
#define SRCS_EVENT_TRANSACTION

#include "event/RequestHandler.hpp"
#include "event/Response.hpp"

class Transaction {
public:
  RequestHandler request_handler_;
  Response       response_;
};

#endif /* SRCS_EVENT_TRANSACTION */
