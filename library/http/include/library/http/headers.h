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

        void add(std::string_view name, std::string_view value);
        std::optional<std::string_view> get_value(std::string_view name) const noexcept;
        const std::vector<THttpHeader>& items() const noexcept;
    private:
        std::vector<THttpHeader> headers_;
    };

    bool check_headers_name_equality(const THttpHeader&, const THttpHeader&);

} //namespace NHttp
