#include <library/common/enum/enum.h>
#include <library/common/split.h>
#include <library/common/parse_number.h>

#include <library/http/error.h>
#include <library/http/model/validate.h>
#include <library/http/request/parser.h>
#include <library/http/request/request.h>

#include <cassert>
#include <cctype>

namespace NHttp {

    THttpRequestParserFeedResult THttpRequestParser::feed(std::string_view data) {
        THttpRequestParserFeedResult result;

        class TDetailParser {
        public:
            explicit TDetailParser(std::string_view data)
                : data_(data)
            {}

            char peek() {
                return data_[position_];
            }

            void next() {
                ++position_;
            }

            size_t pos() {
                return position_;
            }

            void put(size_t new_pos) {
                position_ = new_pos;
            }

            size_t pos_after(size_t count) {
                return std::min(position_ + count, data_.size());
            }

            bool is_eof() {
                return position_ >= data_.size();
            }

            bool can() {
                return can_;
            }

            size_t find(std::string_view str) {
                return data_.find(str , position_);
            }

            size_t find(char symbol) {
                return data_.find(symbol, position_);
            }

            void stop() {
                can_ = false;
            }

            std::string_view substr(size_t last) {
                return data_.substr(position_, last - position_);
            }

            void skip_http_ows() {
                while (is_http_ows(peek())) {
                    next();
                }
            }

            static bool is_http_ows(char c) noexcept {
                return c == ' ' || c == '\t';
            }
        private:
            std::string_view data_;
            size_t position_ = 0;
            bool can_ = true;
        };

        TDetailParser helper(data);

        while (helper.can()) {
            if (state_ == EHttpRequestParserState::REQUEST_LINE) {     
                size_t crlf_pos = helper.find("\r\n");
                if (crlf_pos == std::string_view::npos) {
                    helper.stop();
                    continue;
                }

                //avoid keep alive msg
                if (crlf_pos == helper.pos()) { 
                    helper.put(crlf_pos + 2);
                    continue;
                }

                auto values = NCommon::split(helper.substr(crlf_pos), ' ');
                if (values.size() != 3) {
                    throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", helper.substr(crlf_pos));
                }

                {
                    auto method_opt = NEnum::enum_from_string<EHttpRequestMethod>(values.front());
                    if (!method_opt.has_value()) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST METHOD", values.front());
                    }
                    request_.set_method(method_opt.value());
                }
                
                request_.set_target(std::string(values[1]));
                
                {
                    std::string_view version_str = values.back();
                    if (!version_str.starts_with("HTTP/") || version_str.size() < 8) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST HTTP VERSION", values.back());
                    }

                    auto dot_pos = version_str.find('.', 5);
                    if (dot_pos == std::string_view::npos) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST HTTP VERSION", values.back());
                    }

                    auto major_opt = NCommon::parse_number<uint16_t>(version_str.substr(5, dot_pos - 5));
                    auto minor_opt = NCommon::parse_number<uint16_t>(version_str.substr(dot_pos + 1));
                    if (!major_opt.has_value() || !minor_opt.has_value()) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST HTTP VERSION", values.back());
                    }
                    request_.set_version(THttpVersion(major_opt.value(), minor_opt.value()));
                }

                state_ = EHttpRequestParserState::HEADERS;
                helper.put(crlf_pos + 2);
            } else if (state_ == EHttpRequestParserState::HEADERS) {
                size_t crlf_pos = helper.find("\r\n");
                if (crlf_pos == std::string_view::npos) {
                    helper.stop();
                    continue;
                }

                if (crlf_pos == helper.pos()) { 
                    helper.put(crlf_pos + 2);
                    state_ = EHttpRequestParserState::BODY;
                    continue;
                }

                size_t colon_pos = helper.find(':');
                if (colon_pos == std::string_view::npos || colon_pos > crlf_pos) {
                    throw NError::THttpBadParseHeader("BAD REQUEST HEADER NO COLON", helper.substr(crlf_pos));
                }


                std::string_view name = helper.substr(colon_pos);
                helper.put(colon_pos + 1);
                std::string_view value = helper.substr(crlf_pos);

                request_.headers().add(std::string(name), std::string(NCommon::trim_ows(value)));
                helper.put(crlf_pos + 2);
            } else if (state_ == EHttpRequestParserState::BODY) {
                auto content_length_opt = request_.headers().get_value("Content-Length");
                if (!content_length_opt.has_value()) {
                    state_ = EHttpRequestParserState::COMPLETE;
                    continue;
                }

                auto size_opt = NCommon::parse_number<size_t>(content_length_opt.value());
                if (!size_opt.has_value() || !NModel::validate_body(size_opt.value())) {
                    throw NError::THttpBadParseBodySize();
                }

                request_.reserve_body(size_opt.value());
                assert(size_opt.value() >= request_.body().size());

                size_t left = size_opt.value() - request_.body().size();
                size_t last_pos = helper.pos_after(left);

                request_.add_body(std::string(helper.substr(last_pos)));
                helper.put(last_pos); 

                if (request_.body().size() == size_opt.value()) {
                    state_ = EHttpRequestParserState::COMPLETE;
                } else {
                    helper.stop();
                }
            } else {
                if (!request_.valid()) {
                    throw NError::THttpBadParseResult();
                }
                result.requests.push_back(std::move(request_));
                clear();
            }
        }
        result.parsed_bytes = helper.pos();
        return result;
    }

    std::optional<THttpRequest> THttpRequestParser::request() {
        if (state_ == EHttpRequestParserState::COMPLETE) {
            auto value = std::move(request_);
            clear();
            return value;
        }
        return std::nullopt;
    }

    void THttpRequestParser::clear() {
        request_ = THttpRequest();
        state_ = EHttpRequestParserState::REQUEST_LINE;
    }
  
} // namespace NHttp
