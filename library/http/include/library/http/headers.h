#pragma once 

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace NHttp {

    class THttpHeader {
    public:
        explicit THttpHeader(std::string, std::string);

        const std::string& name() const noexcept;
        const std::string& value() const noexcept;
    private:
        std::string name_;
        std::string value_;
    };

    class THttpHeaders {
    public:
        THttpHeaders() = default;

        void add(std::string name, std::string value);
        std::optional<std::string_view> get_value(std::string_view name) const noexcept;
        std::vector<std::string_view> get_values(std::string_view name) const;
        const std::vector<THttpHeader>& items() const noexcept;
        size_t total_size() const noexcept;
    private:
        std::vector<THttpHeader> headers_;
        size_t total_size_ = 0;        
    };

    bool is_equal_case_insensitive(std::string_view lhs, std::string_view rhs) noexcept;

} // namespace NHttp
