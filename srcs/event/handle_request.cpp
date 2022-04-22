#include "event.hpp"

/*
  データを受信しているfdからデータを読み込み、既存の受信データがある場合データを追加する。
  もしrecvの返り値が0だった場合（コネクションが閉じている）、
  受信したデータをhttpリクエストとしてパース、対応したレスポンスを作成する。
  閉じたコネクションはconnection_listから削除する。
  connection_listはsocket、受信データ(string)を含んだクラスのほうが良いかも。

*/

void handle_request(int connection, const std::map<int, Socket> &socket_list,
                    std::map<int, Connection> &connection_list) {}