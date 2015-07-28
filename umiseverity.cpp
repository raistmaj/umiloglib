#include "umiseverity.hpp"
#include <boost/algorithm/string.hpp>
#include <unordered_map>
#include <string>


std::unordered_map<std::string,umi::Severity> unordered_map_string_to_severity =
{
  {"emergency",umi::Severity::Emergency},
  {"alert",umi::Severity::Alert},
  {"critical",umi::Severity::Critical},
  {"error",umi::Severity::Error},
  {"warning",umi::Severity::Warning},
  {"notice",umi::Severity::Notice},
  {"informational",umi::Severity::Informational},
  {"debug",umi::Severity::Debug}
};

umi::Severity umi::string_to_severity(const std::string &value)
{
  std::string lcValue = boost::to_lower_copy(value);
  const auto element = unordered_map_string_to_severity.find(lcValue);
  if(element == unordered_map_string_to_severity.end()){
    return umi::Severity::Debug;
  }
  return element->second;
}
