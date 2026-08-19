#include <library/http/response/response.h>
#include <library/http/request/request.h>
#include <library/http/error.h>

#include <cstdlib>
#include <string>
#include <utility>

constexpr void static_check(bool value) {
    if (!value) {
        std::exit(EXIT_FAILURE);
    }
}

void check(bool value) {
    if (!value) {
        std::exit(EXIT_FAILURE);
    }
}

namespace NHttp::NTests {

    constexpr void test_enums_compile_time() {
        static_check(static_cast<uint16_t>(EHttpResponseStatus::SWITCHING_PROTOCOLS) == 101);
        static_check(static_cast<uint16_t>(EHttpResponseStatus::OK) == 200);
        static_check(static_cast<uint16_t>(EHttpResponseStatus::CREATED) == 201);
        static_check(static_cast<uint16_t>(EHttpResponseStatus::NO_CONTENT) == 204);
        static_check(static_cast<uint16_t>(EHttpResponseStatus::BAD_REQUEST) == 400);
        static_check(static_cast<uint16_t>(EHttpResponseStatus::INTERNAL_SERVER_ERROR) == 500);

        static_check(static_cast<uint8_t>(EHttpRequestMethod::NOT_SET) == 0);
        static_check(static_cast<uint8_t>(EHttpRequestMethod::GET) == 1);
        static_check(static_cast<uint8_t>(EHttpRequestMethod::POST) == 2);
    }

    void test_http_header_struct() {
        THttpHeader header1{"Content-Type", "application/json"};
        check(header1.name() == "Content-Type");
        check(header1.value() == "application/json");

        std::string name = "Authorization";
        std::string value = "Bearer token123";
        THttpHeader header2{std::move(name), std::move(value)};
        
        check(header2.name() == "Authorization");
        check(header2.value() == "Bearer token123");
    }

    void test_http_headers_class() {
        THttpHeaders headers;
        
        check(headers.items().empty());
        check(!headers.get_value("Content-Type").has_value());

        headers.add("Content-Length", "1024");

        check(headers.get_value("content-length").value() == "1024");
        check(headers.get_value("CONTENT-LENGTH").value() == "1024");
        check(headers.get_value("CoNtEnT-LeNgTh").value() == "1024");
        headers.add("Connection", "keep-alive");

        check(headers.items().size() == 2);

        auto contentLength = headers.get_value("Content-Length");
        check(contentLength.has_value());
        check(contentLength.value() == "1024");

        auto connection = headers.get_value("Connection");
        check(connection.has_value());
        check(connection.value() == "keep-alive");

        check(!headers.get_value("Host").has_value());
    }

    void test_http_request() {
        THttpRequest request;

        request.set_method(EHttpRequestMethod::POST);
        check(request.method() == EHttpRequestMethod::POST);

        request.set_target("/api/v1/trade/order");
        check(request.target() == "/api/v1/trade/order");

        std::string bodyPayload = R"({"symbol": "BTCUSD", "qty": 1.5})";
        request.set_body(bodyPayload);
        check(request.body() == bodyPayload);

        request.headers().add("Content-Type", "application/json");
        request.headers().add("X-Request-ID", "req-777");

        const auto& constReq = request;
        check(constReq.headers().items().size() == 2);
        check(constReq.headers().get_value("X-Request-ID").value() == "req-777");
    }

    void test_http_response() {
        THttpResponse response;

        response.set_status(EHttpResponseStatus::NOT_FOUND);
        check(response.status() == EHttpResponseStatus::NOT_FOUND);

        response.set_status(EHttpResponseStatus::OK);
        check(response.status() == EHttpResponseStatus::OK);

        check(response.version().major == 1);
        check(response.version().minor == 1);
        
        response.set_body("<html><body>Success</body></html>");
        check(response.body() == "<html><body>Success</body></html>");

        response.headers().add("Server", "Yandex Cloud / C++ HFT");
        response.headers().add("Content-Length", std::to_string(response.body().size()));

        check(response.headers().get_value("Server").value() == "Yandex Cloud / C++ HFT");
        check(response.headers().get_value("Content-Length").value() == "33");
    }

    void run_all_tests() {
        test_enums_compile_time();
        test_http_header_struct();
        test_http_headers_class();
        test_http_request();
        test_http_response();
    }

} // namespace NHttp::NTests

int main() {
    NHttp::NTests::run_all_tests();
    return 0;
}
