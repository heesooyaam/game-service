#pragma once

#include <library/http/request/request.h>

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace NHttp {

    struct THttpRequestParserFeedResult {
        std::vector<THttpRequest> requests;
        size_t parsed_bytes = 0;
    };

    class THttpRequestParser {
    public:
        THttpRequestParser() = default;    

        THttpRequestParser(const THttpRequestParser&) = delete;
        THttpRequestParser& operator=(const THttpRequestParser&) = delete;

        THttpRequestParser(THttpRequestParser&&) = delete;
        THttpRequestParser& operator=(THttpRequestParser&&) = delete;

        THttpRequestParserFeedResult feed(std::string_view data);
    private:
        enum class EHttpRequestParserState : uint8_t {
            KEEP_ALIVE = 0,
            METHOD,
            TARGET,
            VERSION,
            HEADER_NAME,
            HEADER_VALUE,
            BODY,
            COMPLETE
        };

        THttpRequest request_;
        size_t last_position_ = 0;
        size_t parsed_bytes_current_state_ = 0;
        std::optional<std::string> name_opt_ = std::nullopt;
        EHttpRequestParserState state_ = EHttpRequestParserState::KEEP_ALIVE;

        void clear();
    };

} // namespace NHttp
