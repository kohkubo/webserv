#ifndef SRCS_HTTP_HEADERFIELDMAP_HPP
#define SRCS_HTTP_HEADERFIELDMAP_HPP

#include <map>

class HeaderFieldMap {
public:
  typedef std::map<std::string, std::string> field_map_type;

private:
  field_map_type           _field_map_;
  static const std::string OWS_;

public:
  HeaderFieldMap() {}
  ~HeaderFieldMap() {}
  bool                           store_new_field(std::string &field_line);
  field_map_type::iterator       find(const std::string &field_name);
  field_map_type::const_iterator find(const std::string &field_name) const;
  field_map_type::iterator       end() { return _field_map_.end(); }
  field_map_type::const_iterator end() const { return _field_map_.end(); }
};

#endif /* SRCS_HTTP_HEADERFIELDMAP_HPP */
