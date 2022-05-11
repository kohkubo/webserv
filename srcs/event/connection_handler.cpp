#include "config/ServerConfig.hpp"
#include "event/Connection.hpp"
#include "utils/utils.hpp"
#include <sys/socket.h>
#include <sys/types.h>

/*
  コネクションが持つリクエストのキューの状態として、、、
  1, リクエストを一つも持たない。
  2, リクエストがあり、キューの最後のリクエストは読み取り済み。
  (状態1, 2はデータを受信したら、状態3へ移行する）
  3, リクエストがあり、キューの最後のリクエストがヘッダーを受信中
  4, リクエストがあり、キューの最後のリクエストがbodyを受信中

continue -> データ残あり、loop継続
break -> データ残なし、loop終了
while (it != data.end())
  switch state:
    case 新しいリクエスト or 最後のリクエストは読み取り済み(状態1,2)
      新しいリクエストをキューに追加 continue
    case リクエストがヘッダーを受信中(状態3)
      if データにヘッダーの区切り文字がある
        ヘッダーに区切り文字までを格納、ヘッダーが完成するのでパース
          if リクエストにbodyがある
            状態4へ移行 continue
          else
            状態2へ移行 continue
      else
        データを全てヘッダーに格納 break
    case リクエストがbodyを受信中(状態4)
      if データがbodyサイズより大きい(chunkedのときは終了指定までパースできた)
        データをbodyに格納。状態2へ移行 continue
      else
        データを全てbodyに格納 break
 */

// std::string(std::vector<char>)でnull文字交じりのstring作れるかも

static std::string receive_data(int accfd) {
  const int buf_size      = 1024;
  char      buf[buf_size] = {};
  ssize_t   rc            = recv(accfd, buf, buf_size - 1, MSG_DONTWAIT);
  if (rc == -1) {
    error_log_with_errno("recv() failed.");
    return std::string();
  }
  // TODO: rc == 0 => connection close
  return std::string(buf);
}

// ヘッダーの区切り(rnrn）を探す
static bool find_header_delim(std::string &data) {
  const std::string delim("\r\n\r\n");
  if (data.find(delim) != std::string::npos)
    return true;
  return false;
}

void connection_handler(int accfd, const connection_list_type &tcp,
                        const socket_list_type &socket_list) {
  // TODO: 適切なServerConfigを選択する。
  (void)socket_list;

  std::string data = receive_data(accfd);
}