#include "http/HeaderFieldMap.hpp"

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
  if (_field_map_.count(field_name) == 1u) {
    ERROR_LOG("store_new_field: duplicated field.");
    return false;
  }
  // TODO: error handling
  _field_map_[field_name] = field_value;
  return true;
}

HeaderFieldMap::field_map_type::iterator
HeaderFieldMap::find(const std::string &field_name) {
  return _field_map_.find(field_name);
}

HeaderFieldMap::field_map_type::const_iterator
HeaderFieldMap::find(const std::string &field_name) const {
  return _field_map_.find(field_name);
}

bool HeaderFieldMap::has_field(const std::string &field_name) const {
  return _field_map_.count(field_name) == 1u;
}
