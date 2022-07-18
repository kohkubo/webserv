#ifndef SRCS_HTTP_HEADERFIELDMAP_HPP
#define SRCS_HTTP_HEADERFIELDMAP_HPP

#include <map>
#include <string>

class HeaderFieldMap {
public:
  typedef std::map<std::string, std::string> field_map_type;

private:
  field_map_type           _field_map_;
  static const std::string OWS_;

public:
  HeaderFieldMap() {}
  ~HeaderFieldMap() {}
  bool               store_new_field(std::string &field_line);
  const std::string &value(const std::string &field_name) const;
  bool               has_field(const std::string &field_name) const;
  std::string        to_string() const;
};

#endif /* SRCS_HTTP_HEADERFIELDMAP_HPP */
