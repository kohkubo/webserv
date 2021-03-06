#include "http/HeaderFieldMap.hpp"

#include "http/const/const_delimiter.hpp"
#include "utils/utils.hpp"

const std::string HeaderFieldMap::OWS_ = " \t";

/*
Rfc9110 sec5.3
A recipient MAY combine multiple field lines within a field section that have
the same field name into one field line, without changing the semantics of the
message, by appending each subsequent field line value to the initial field line
value in order, separated by a comma (",") and optional whitespace (OWS, defined
in Section 5.6.3). For consistency, use comma SP.
カンマ区切りのフィールドを結合できる（MAY)なので重複するフィールドはエラー扱いで
 */

bool HeaderFieldMap::store_new_field(std::string &field_line) {
  std::size_t pos = field_line.find(':');
  if (pos == std::string::npos) {
    ERROR_LOG("store_new_field: invalid header line.");
    return false;
  }
  std::string field_name = tolower(field_line.substr(0, pos));
  char        last_char  = field_name[field_name.size() - 1];
  if (last_char == ' ' || last_char == '\t') {
    ERROR_LOG("store_new_field: field name has OWS.");
    return false;
  }
  const std::string field_value = trim(field_line.substr(pos + 1), OWS_);
  if (field_value.empty()) {
    ERROR_LOG("store_new_field: empty field value.");
    return false;
  }
  std::pair<field_map_type::iterator, bool> result =
      _field_map_.insert(std::make_pair(field_name, field_value));
  if (!result.second) {
    ERROR_LOG("store_new_field: field name is duplicated.");
    return false;
  }
  return true;
}

// has_fieldでkeyがあることを確認せずに使用しない。
const std::string &HeaderFieldMap::value(const std::string &field_name) const {
  return _field_map_.find(field_name)->second;
}

bool HeaderFieldMap::has_field(const std::string &field_name) const {
  return _field_map_.count(field_name) == 1u;
}

std::string HeaderFieldMap::to_string() const {
  std::string                    res;
  field_map_type::const_iterator it = _field_map_.begin();
  for (; it != _field_map_.end(); it++) {
    res += it->first + ": " + it->second + CRLF;
  }
  return res;
}