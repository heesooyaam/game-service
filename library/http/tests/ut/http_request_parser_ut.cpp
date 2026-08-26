#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/http/error.h>
#include <library/http/request/parser.h>
#include <library/http/request/request.h>

#include <string>

namespace NHttp::NTests {

    TEST_CASE(test_http_parser_exceptions) {

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / \r\n"), 
                NError::THttpBadParseRequestLine
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/11\r\n"), 
                NError::THttpBadParseRequestLine
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/1.1\r\nHost localhost\r\n\r\n"), 
                NError::THttpBadParseHeader
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/1.1\r\nHost : localhost\r\n\r\n"), 
                NError::THttpBadHeaderName
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 10A\r\n\r\n"), 
                NError::THttpBadParseBodySize
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("POST / HTTP/1.1\r\nHost: localhost\r\nContent-Length: 5\r\nTransfer-Encoding: chunked\r\n\r\nHello"), 
                NError::THttpBadParseResult
            );
        }
        
        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/1.1\r\nAccept: */*\r\n\r\n"),
                NError::THttpBadParseResult
            );
        }
    }

    TEST_CASE(test_http_parser_keep_alive) {
        THttpRequestParser parser;
        
        std::string raw_data = "\r\n\r\nGET /api HTTP/1.1\r\nHost: ya.ru\r\n\r\n";
        auto result = parser.feed(raw_data);

        CHECK(result.requests.size() == 1);
        CHECK(result.requests[0].method() == EHttpRequestMethod::GET);
        CHECK(result.requests[0].target() == "/api");
        CHECK(result.parsed_bytes == raw_data.size()); 
    }

    TEST_CASE(test_http_parser_happy_path_and_ows) {
        THttpRequestParser parser;

        std::string raw_data = 
            "POST /submit HTTP/1.1\r\n"
            "Host:    localhost    \t\r\n"  
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "Hello, World!";

        auto result = parser.feed(raw_data);

        CHECK(result.requests.size() == 1);
        CHECK(result.parsed_bytes == raw_data.size());

        auto& req = result.requests[0];
        CHECK(req.method() == EHttpRequestMethod::POST);
        CHECK(req.target() == "/submit");
        CHECK(req.version().major == 1);
        CHECK(req.version().minor == 1);
        
        CHECK(req.headers().get_value("Host").value() == "localhost");
        CHECK(req.body() == "Hello, World!");
    }

    // 4. ТЕСТИРОВАНИЕ СКЛЕЙКИ ЗАПРОСОВ (Pipelining)
    TEST_CASE(test_http_parser_pipelining) {
        THttpRequestParser parser;

        std::string raw_data = 
            "GET /1 HTTP/1.1\r\nHost: a\r\n\r\n"
            "GET /2 HTTP/1.1\r\nHost: b\r\n\r\n"
            "POST /3 HTTP/1.1\r\nHost: c\r\nContent-Length: 2\r\n\r\nOK";

        auto result = parser.feed(raw_data);

        CHECK(result.requests.size() == 3);
        CHECK(result.parsed_bytes == raw_data.size());

        CHECK(result.requests[0].target() == "/1");
        CHECK(result.requests[1].target() == "/2");
        CHECK(result.requests[2].target() == "/3");
        CHECK(result.requests[2].body() == "OK");
    }

    TEST_CASE(test_http_parser_fragmentation_and_bytes) {
        THttpRequestParser parser;
        std::string socket_buffer;

        socket_buffer += "GET /full HTTP/1.1\r\nHost: a\r\n\r\nPO";
        
        auto result1 = parser.feed(socket_buffer);
        CHECK(result1.requests.size() == 1);
        CHECK(result1.requests[0].target() == "/full");
        
        size_t expected_parsed = std::string("GET /full HTTP/1.1\r\nHost: a\r\n\r\n").size();
        CHECK(result1.parsed_bytes == expected_parsed);

        socket_buffer.erase(0, result1.parsed_bytes);
        CHECK(socket_buffer == "PO");

        socket_buffer += "ST /partial HTTP/1.1\r\nHost: b\r\nContent-Length: 10\r\n\r\n12345";
        
        auto result2 = parser.feed(socket_buffer);
        CHECK(result2.requests.size() == 0); 
        CHECK(result2.parsed_bytes == socket_buffer.size()); 
        
        socket_buffer.erase(0, result2.parsed_bytes);
        CHECK(socket_buffer.empty());

        socket_buffer += "67890";
        
        auto result3 = parser.feed(socket_buffer);
        CHECK(result3.requests.size() == 1);
        CHECK(result3.parsed_bytes == socket_buffer.size());
        
        CHECK(result3.requests[0].target() == "/partial");
        CHECK(result3.requests[0].body() == "1234567890");
    }

    TEST_CASE(test_http_parser_target_validation) {

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET api/v1/users HTTP/1.1\r\nHost: ya.ru\r\n\r\n"),
                NError::THttpBadTarget
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET /path%2G HTTP/1.1\r\nHost: ya.ru\r\n\r\n"),
                NError::THttpBadTarget
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET /path%2 HTTP/1.1\r\nHost: ya.ru\r\n\r\n"),
                NError::THttpBadTarget
            );
        }
        
        THttpRequestParser parser;
        auto result = parser.feed("GET /path%20with%20spaces HTTP/1.1\r\nHost: ya.ru\r\n\r\n");
        CHECK(result.requests.size() == 1);
        CHECK(result.requests[0].target() == "/path%20with%20spaces");
    }

    TEST_CASE(test_http_parser_get_with_body_rules) {
        {
            THttpRequestParser parser;
            auto result = parser.feed("GET / HTTP/1.1\r\nHost: ya.ru\r\nContent-Length: 0\r\n\r\n");
            CHECK(result.requests.size() == 1);
            CHECK(result.requests[0].body().empty());
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/1.1\r\nHost: ya.ru\r\nContent-Length: 5\r\n\r\nHello"),
                NError::THttpBadParseResult
            );
        }
    }

    TEST_CASE(test_http_parser_multiple_and_empty_headers) {
        THttpRequestParser parser;

        std::string raw_data = 
            "GET / HTTP/1.1\r\n"
            "Host: ya.ru\r\n"
            "Accept: text/html\r\n"
            "Accept: application/json\r\n"
            "X-Empty-Header: \r\n" 
            "\r\n";

        auto result = parser.feed(raw_data);
        CHECK(result.requests.size() == 1);

        const auto& headers = result.requests[0].headers();

        auto accept_values = headers.get_values("Accept");
        CHECK(accept_values.size() == 2);
        CHECK(accept_values[0] == "text/html");
        CHECK(accept_values[1] == "application/json");

        auto empty_val = headers.get_value("X-Empty-Header");
        CHECK(empty_val.has_value());
        CHECK(empty_val.value() == "");
    }

    TEST_CASE(test_http_parser_smuggling_protection) {

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("POST / HTTP/1.1\r\nHost: ya.ru\r\nContent-Length: 5\r\nContent-Length: 5\r\n\r\n12345"),
                NError::THttpBadParseResult
            );
        }
        
        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("POST / HTTP/1.1\r\nHost: ya.ru\r\nContent-Length: 5\r\nContent-Length: 10\r\n\r\n12345"),
                NError::THttpBadParseResult
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("POST / HTTP/1.1\r\nHost: ya.ru\r\nContent-Length: 1 0\r\n\r\n1234567890"),
                NError::THttpBadParseBodySize
            );
        }

        {
            THttpRequestParser parser;
            CHECK_THROWS_AS(
                parser.feed("GET / HTTP/1.1\r\nHost: ya.ru 8080\r\n\r\n"),
                NError::THttpBadParseResult
            );
        }
        
    }

    TEST_CASE(test_http_parser_fragmentation_headers) {
        THttpRequestParser parser;
        std::string socket_buffer;

        socket_buffer += "POST /api/v1/use";
        auto res1 = parser.feed(socket_buffer);
        CHECK(res1.requests.empty());
        CHECK(res1.parsed_bytes == 0);

        socket_buffer += "rs HTTP/1.1\r\nHost: ya";
        auto res2 = parser.feed(socket_buffer);
        CHECK(res2.requests.empty());
        CHECK(res2.parsed_bytes == std::string("POST /api/v1/use").size() + std::string("rs HTTP/1.1\r\n").size());
        socket_buffer.erase(0, res2.parsed_bytes);
        
        CHECK(socket_buffer == "Host: ya");
        socket_buffer += ".ru\r\nContent-Length: 5\r\n\r\n12";
        auto res3 = parser.feed(socket_buffer);
        CHECK(res3.requests.empty());
        CHECK(res3.parsed_bytes == socket_buffer.size());
        socket_buffer.erase(0, res3.parsed_bytes);
        CHECK(socket_buffer.empty());

        socket_buffer += "345";
        auto res4 = parser.feed(socket_buffer);
        
        CHECK(res4.requests.size() == 1);
        CHECK(res4.parsed_bytes == socket_buffer.size());
        
        CHECK(res4.requests[0].method() == EHttpRequestMethod::POST);
        CHECK(res4.requests[0].target() == "/api/v1/users");
        CHECK(res4.requests[0].headers().get_value("Host").value() == "ya.ru");
        CHECK(res4.requests[0].body() == "12345");
    }

    TEST_CASE(test_http_parser_fragmentation_crlf_edge) {
        THttpRequestParser parser;
        std::string socket_buffer;

        socket_buffer += "GET / HTTP/1.1\r";
        auto res1 = parser.feed(socket_buffer);
        CHECK(res1.requests.empty());
        CHECK(res1.parsed_bytes == 0);

        socket_buffer += "\nHost: a\r";
        auto res2 = parser.feed(socket_buffer);
        CHECK(res2.requests.empty());
        CHECK(res2.parsed_bytes == std::string("GET / HTTP/1.1\r").size() + std::string("\n").size());
        socket_buffer.erase(0, res2.parsed_bytes);

        socket_buffer += "\n\r\n";
        auto res3 = parser.feed(socket_buffer);
        CHECK(res3.requests.size() == 1);
        CHECK(res3.parsed_bytes == socket_buffer.size());
        CHECK(res3.requests[0].target() == "/");
    }

    TEST_CASE(test_http_parser_byte_by_byte) {
        THttpRequestParser parser;
        std::string full_request = "POST / HTTP/1.1\r\nHost: a\r\nContent-Length: 2\r\n\r\nOK";
        std::string socket_buffer;

        bool request_ready = false;

        for (size_t i = 0; i < full_request.size(); ++i) {
            socket_buffer += full_request[i];
            auto result = parser.feed(socket_buffer);
            
            socket_buffer.erase(0, result.parsed_bytes);
            
            if (!result.requests.empty()) {
                CHECK(result.requests.size() == 1);
                CHECK(i == full_request.size() - 1); 
                CHECK(result.requests[0].body() == "OK");
                request_ready = true;
            } else {
                CHECK(request_ready == false); 
            }
        }
        
        CHECK(request_ready); 
    }

} // namespace NHttp::NTests
