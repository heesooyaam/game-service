#include <library/common/enum/enum.h>
#include <library/common/split.h>
#include <library/common/parse_number.h>

#include <library/http/error.h>
#include <library/http/model/validate.h>
#include <library/http/request/parser.h>
#include <library/http/request/request.h>

#include <cassert>
#include <cctype>
#include <string_view>
#include <format>

namespace NHttp {

    THttpRequestParserFeedResult THttpRequestParser::feed(std::string_view data) {
        THttpRequestParserFeedResult result;

        class TDetailParser {
        public:
            explicit TDetailParser(std::string_view data, size_t position, size_t& parsed_bytes)
                : data_(data)
                , current_position_(position)
                , previous_position_(0)
                , stop_(false)
            {
                if (position > 0 && data[position - 1] == '\r') {
                    --current_position_;
                
                    assert(parsed_bytes != 0);
                    --parsed_bytes;
                }
            }

            bool work() const noexcept {
                return stop_ == false;
            }

            void stop() noexcept {
                stop_ = true;
            }

            size_t current_position() const noexcept {
                return current_position_;
            }

            size_t previos_position() const noexcept {
                return previous_position_;
            }

            void put_at_position(size_t new_position) noexcept {
                current_position_ = new_position;
                previous_position_ = new_position;
            }

            void move_on(size_t counter) noexcept {
                current_position_ += counter;
                previous_position_ += counter;
            }

            void put_current_position(size_t new_position) noexcept {
                current_position_ = new_position;
            }

            bool is_crlf() {
                if (current_position_ + 1 >= data_.size()) {
                    return false;
                }
                return data_[current_position_] == '\r' && data_[current_position_ + 1] == '\n';
            }

            bool is_eof() const {
                assert(current_position_ <= data_.size());
                return current_position_ == data_.size();
            }

    
            std::pair<size_t, size_t> find_char_or_crlf(char ch, size_t size_search) {
                std::string_view view = substr_to_find(size_search);
                for (size_t i = 0; i < view.size(); ++i) {
                    if (view[i] == ch) {
                        return std::make_pair(current_position_ + i, std::string_view::npos);
                    }
                    if (view[i] == '\r' && i + 1 < view.size() && view[i + 1] == '\n') {
                        return std::make_pair(std::string_view::npos, current_position_ + i);
                    }
                }
                return {std::string_view::npos, std::string_view::npos};
            }

            size_t find(std::string_view str, size_t size_search) {
                size_t idx = substr_to_find(size_search).find(str);
                if (idx != std::string_view::npos) {
                    idx += current_position_;
                }
                return idx;
            }

            size_t find(char ch, size_t size_search) {
                size_t idx = substr_to_find(size_search).find(ch);
                if (idx != std::string_view::npos) {
                    idx += current_position_;
                }
                return idx;
            }

            std::string_view substr_to_find(size_t max_size) {
                assert(current_position_ + max_size <= data_.size());
                return data_.substr(current_position_,max_size);
            }

            std::string_view substr(size_t last) {
                assert(previous_position_ <= last);
                return data_.substr(previous_position_, last - previous_position_);
            }

            size_t pos_after(size_t count) {
                return std::min(current_position_ + count, data_.size());
            }

        private:
            std::string_view data_;
            size_t current_position_;
            size_t previous_position_;
            bool stop_;
        };

        TDetailParser parser(data, last_position_, parsed_bytes_current_state_);

        while (parser.work()) {
            if (state_ == EHttpRequestParserState::KEEP_ALIVE) {
                while (!parser.is_eof() && parser.is_crlf()) {
                    parser.move_on(2);
                    parsed_bytes_current_state_ += 2;

                    if (parsed_bytes_current_state_ > NModel::MAX_REQUEST_LINE_SIZE_BYTES) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", "TOO LONG"); 
                    }
                }

                if (parser.is_eof()) {
                    parser.stop();
                    continue;
                }

                if (parser.current_position() + 1 == data.size() && data[parser.current_position()] == '\r') {
                    parser.stop();
                    continue;
                }

                state_ = EHttpRequestParserState::METHOD;
            } else if (state_ == EHttpRequestParserState::METHOD || state_ == EHttpRequestParserState::TARGET) {
                assert(NModel::MAX_REQUEST_LINE_SIZE_BYTES >= parsed_bytes_current_state_);

                size_t size_left = NModel::MAX_REQUEST_LINE_SIZE_BYTES - parsed_bytes_current_state_;
                size_t size_available = data.size() - parser.current_position();

                if (size_left == 0) {
                    throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", "TOO LONG"); 
                }

                size_t search_len = std::min(size_available, size_left);
                auto [space_pos, crlf_pos] = parser.find_char_or_crlf(' ', search_len);

                if (crlf_pos != std::string_view::npos) {
                    std::string_view parsing = "METHOD";
                    if (state_ == EHttpRequestParserState::TARGET) {
                        parsing = "TARGET";
                    }
                    throw NError::THttpBadParseRequestLine(
                        std::format(
                            "BAD REQUEST LINE NO SPACE TO PARSE {}", 
                            parsing
                        ), 
                        parser.substr(
                            std::min(data.size(), crlf_pos)
                        )
                    ); 
                }
                
                if (space_pos == std::string_view::npos) {
                    if (search_len == size_left) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", "TOO LONG");
                    }
                    parsed_bytes_current_state_ += size_available;
                    parser.put_current_position(data.size());
                    parser.stop();
                    continue;
                } 
                    
                if (state_ == EHttpRequestParserState::METHOD) {
                    std::string_view method = parser.substr(space_pos);
                    auto method_opt = NEnum::enum_from_string<EHttpRequestMethod>(method);
                    if (!method_opt.has_value()) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST METHOD", method);
                    }
                    
                    request_.set_method(method_opt.value());
                    state_ = EHttpRequestParserState::TARGET;                
                } else {
                    std::string_view target = parser.substr(space_pos);
                    request_.set_target(std::string(target));
                    state_ = EHttpRequestParserState::VERSION;
                }
                parsed_bytes_current_state_ += (space_pos + 1) - parser.current_position();
                parser.put_at_position(space_pos + 1);
            } else if (state_ == EHttpRequestParserState::VERSION) {
                assert(NModel::MAX_REQUEST_LINE_SIZE_BYTES >= parsed_bytes_current_state_);

                size_t size_left = NModel::MAX_REQUEST_LINE_SIZE_BYTES - parsed_bytes_current_state_;
                size_t size_available = data.size() - parser.current_position();

                if (size_left == 0) {
                    throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", "TOO LONG"); 
                }

                size_t search_len = std::min(size_available, size_left);
                size_t crlf_pos = parser.find("\r\n", search_len);

                if (crlf_pos == std::string::npos) {
                    if (search_len == size_left) {
                        throw NError::THttpBadParseRequestLine("BAD REQUEST LINE", "TOO LONG");
                    }
                    parsed_bytes_current_state_ += size_available;
                    parser.put_current_position(data.size());
                    parser.stop();
                    continue;
                } 

                std::string_view version = parser.substr(crlf_pos);
                if (version != "HTTP/1.1") {
                    throw NError::THttpBadParseRequestLine("BAD REQUEST HTTP VERSION", version);
                } 
                request_.set_version(THttpVersion());
                parser.put_at_position(crlf_pos + 2);
                state_ = EHttpRequestParserState::HEADER_NAME;
                parsed_bytes_current_state_ = 0;
            } else if (state_ == EHttpRequestParserState::HEADER_NAME) {
                assert(NModel::MAX_HEADERS_SIZE_BYTES >= parsed_bytes_current_state_);

                size_t size_left = NModel::MAX_HEADERS_SIZE_BYTES - parsed_bytes_current_state_;
                size_t size_available = data.size() - parser.current_position();

                if (size_left == 0) {
                    throw NError::THttpBadParseHeader("BAD HEADER", "TOO LONG"); 
                }

                size_t search_len = std::min(size_available, size_left);
                auto [colon_pos, crlf_pos] = parser.find_char_or_crlf(':', search_len);                

                if (parser.previos_position() == crlf_pos) {
                    parser.put_at_position(crlf_pos + 2);
                    state_ = EHttpRequestParserState::BODY;
                    parsed_bytes_current_state_ = 0;
                    continue;
                }

                if (crlf_pos != std::string_view::npos) {
                    throw NError::THttpBadParseHeader(
                        "BAD REQUEST HEADER NO COLON", 
                        parser.substr(
                            std::min(data.size(), crlf_pos)
                        )
                    );
                }

                if (colon_pos == std::string_view::npos) {
                    if (search_len == size_left) {
                        throw NError::THttpBadParseHeader("BAD HEADER", "TOO LONG"); 
                    }
                    parsed_bytes_current_state_ += size_available;
                    parser.put_current_position(data.size());
                    parser.stop();
                    continue;
                } 

                std::string_view name = parser.substr(colon_pos);
                name_opt_ = name;

                parsed_bytes_current_state_ += (colon_pos + 1) - parser.current_position();
                parser.put_at_position(colon_pos + 1);
                state_ = EHttpRequestParserState::HEADER_VALUE;
            } else if (state_ == EHttpRequestParserState::HEADER_VALUE) {
                assert(NModel::MAX_HEADERS_SIZE_BYTES >= parsed_bytes_current_state_);

                size_t size_left = NModel::MAX_HEADERS_SIZE_BYTES - parsed_bytes_current_state_;
                size_t size_available = data.size() - parser.current_position();

                if (size_left == 0) {
                    throw NError::THttpBadParseHeader("BAD HEADER", "TOO LONG"); 
                }

                size_t search_len = std::min(size_available, size_left);
                size_t crlf_pos = parser.find("\r\n", search_len);

                if (crlf_pos == std::string::npos) {
                    if (search_len == size_left) {
                        throw NError::THttpBadParseHeader("BAD HEADER", "TOO LONG"); 
                    }
                    parsed_bytes_current_state_ += size_available;
                    parser.put_current_position(data.size());
                    parser.stop();
                    continue;
                } 

                std::string_view value = parser.substr(crlf_pos);
                assert(name_opt_.has_value());

                parsed_bytes_current_state_ += (crlf_pos + 2) - parser.current_position();
                parser.put_at_position(crlf_pos + 2);
                state_ = EHttpRequestParserState::HEADER_NAME;

                request_.headers().add(name_opt_.value(), std::string(NCommon::trim_ows(value)));                name_opt_.reset();
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
                size_t last_pos = parser.pos_after(left);

                request_.add_body(std::string(parser.substr(last_pos)));
                parser.put_at_position(last_pos); 

                if (request_.body().size() == size_opt.value()) {
                    state_ = EHttpRequestParserState::COMPLETE;
                } else {
                    parser.stop();
                }
            } else if (state_ == EHttpRequestParserState::COMPLETE) {
                if (!request_.valid()) {
                    throw NError::THttpBadParseResult();
                }
                result.requests.push_back(std::move(request_));
                clear();
            }
        }
        result.parsed_bytes = parser.previos_position();
        last_position_ = parser.current_position() - parser.previos_position();
        return result;
    }

    void THttpRequestParser::clear() {
        request_ = THttpRequest();
        parsed_bytes_current_state_ = 0;
        state_ = EHttpRequestParserState::KEEP_ALIVE;
        assert(name_opt_.has_value() == false);
    }
  
} // namespace NHttp
