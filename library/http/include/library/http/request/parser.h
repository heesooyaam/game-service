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
        std::optional<THttpRequest> request();
    private:
        enum class EHttpRequestParserState : uint8_t {
            REQUEST_LINE = 0,
            HEADERS,
            BODY,
            COMPLETE,
        };

        THttpRequest request_;
        EHttpRequestParserState state_ = EHttpRequestParserState::REQUEST_LINE;

        void clear();
    };

} // namespace NHttp
