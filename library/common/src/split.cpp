#include <library/common/split.h>

namespace NCommon {

    std::vector<std::string_view> split(std::string_view data, char separator) {
        std::vector<std::string_view> result;
        size_t prev_pointer = 0, current_pointer = 0;
        while (current_pointer < data.size()) {
            if (data[current_pointer] == separator) {
                result.emplace_back(trim_ows(data.substr(prev_pointer, current_pointer - prev_pointer)));
                prev_pointer = current_pointer + 1;
            }
            ++current_pointer;
        }
        result.emplace_back(trim_ows(data.substr(prev_pointer, current_pointer - prev_pointer)));
        return result;
    }

} // namespace NCommon
