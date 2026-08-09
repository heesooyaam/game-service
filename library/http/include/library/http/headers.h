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
        const std::vector<THttpHeader>& items() const noexcept;
    private:
        std::vector<THttpHeader> headers_;
    };

} //namespace NHttp
