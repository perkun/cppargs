#include <sstream>

namespace utils {

template <typename T>
T convert_value(std::string source_value)
{
    T converted_value;
    std::stringstream ss;
    ss << source_value;
    ss >> converted_value;
    return converted_value;
}

}  // namespace utils
