#include "http/request/RequestInfo.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

#include "http/const/const_delimiter.hpp"
#include "utils/tokenize.hpp"
#include "utils/utils.hpp"

const std::string RequestInfo::OWS_ = " \t";

static std::string parse_request_host(const std::string &host_line) {
  std::size_t pos = host_line.find(':');
  if (pos == std::string::npos) {
    return host_line;
  }
  return host_line.substr(0, pos);
}

static bool parse_request_connection(const std::string &connection) {
  return tolower(connection) == "close";
}

static size_t parse_request_content_length(const std::string &content_length) {
  Result<std::size_t> result = string_to_size(content_length);
  if (result.is_err_) {
    throw RequestInfo::BadRequestException();
  }
  return result.object_;
}

void RequestInfo::parse_request_header() {
  if (header_field_map_.has_field("host")) {
    const std::string &value = header_field_map_.value("host");
    host_                    = parse_request_host(value);
  } else {
    throw BadRequestException(HttpStatusCode::C_400_BAD_REQUEST,
                              "Host field is not found.");
  }
  if (header_field_map_.has_field("connection")) {
    const std::string &value = header_field_map_.value("connection");
    connection_close_        = parse_request_connection(value);
  }
  if (header_field_map_.has_field("content-length")) {
    const std::string &value = header_field_map_.value("content-length");
    has_content_length_      = true;
    content_length_          = parse_request_content_length(value);
  }
  if (header_field_map_.has_field("transfer-encoding")) {
    // TODO: Transfer-Encodingはカンマ区切りのフィールド
    transfer_encoding_ = header_field_map_.value("transfer-encoding");
  }
  if (header_field_map_.has_field("content-type")) {
    content_type_ = header_field_map_.value("content-type");
  }
}

/*
TODO: 全部400で返すのはRFCに反する? 2022/06/25 kohkubo
RFC 9110
5.4. Field Limits
A server that receives a request header field line, field value, or set of
fields larger than it wishes to process MUST respond with an appropriate 4xx
(Client Error) status code.
要求ヘッダーのフィールド行、フィールド値、または処理したいよりも大きいフィールドのセットを受信するサーバーは、
適切な4xx（クライアントエラー）ステータスコードで応答する必要があります。
*/
/*
RFC 9110
5.3.  Field Order
A server MUST NOT apply a request to the target resource until it receives the
entire request header section, since later header field lines might include
conditionals, authentication credentials, or deliberately misleading duplicate
header fields that could impact request processing.
サーバーは、リクエストヘッダーセクション全体を受信するまで、ターゲットリソースにリクエストを適用してはなりません。
これは、後のヘッダーフィールド行に、条件、認証クレデンシャル、またはリクエスト処理に影響を与える可能性のある意図的に誤解を招く重複ヘッダーフィールドが含まれる可能性があるためです。

TODO: この部分を読むと、ヘッダーごとにパースしても問題ない？ kohkubo
*/
