#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/http/response/response.h>
#include <library/http/request/request.h>
#include <library/http/error.h>

#include <cstdlib>

namespace NHttp::NTests {

    TEST_CASE(test_enums_compile_time) {
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::SWITCHING_PROTOCOLS) == 101);
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::OK) == 200);
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::CREATED) == 201);
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::NO_CONTENT) == 204);
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::BAD_REQUEST) == 400);
        STATIC_CHECK(static_cast<uint16_t>(EHttpResponseStatus::INTERNAL_SERVER_ERROR) == 500);

        STATIC_CHECK(static_cast<uint8_t>(EHttpRequestMethod::NOT_SET) == 0);
        STATIC_CHECK(static_cast<uint8_t>(EHttpRequestMethod::GET) == 1);
        STATIC_CHECK(static_cast<uint8_t>(EHttpRequestMethod::POST) == 2);
    }

    TEST_CASE(test_http_header_struct) {
        THttpHeader header1{"Content-Type", "application/json"};
        CHECK(header1.name() == "Content-Type");
        CHECK(header1.value() == "application/json");

        std::string name = "Authorization";
        std::string value = "Bearer token123";
        THttpHeader header2{std::move(name), std::move(value)};

        CHECK(header2.name() == "Authorization");
        CHECK(header2.value() == "Bearer token123");
    }

    TEST_CASE(test_http_headers_class) {
        THttpHeaders headers;

        CHECK(headers.items().empty());
        CHECK(!headers.get_value("Content-Type").has_value());

        headers.add("Content-Length", "1024");

        CHECK(headers.get_value("content-length").value() == "1024");
        CHECK(headers.get_value("CONTENT-LENGTH").value() == "1024");
        CHECK(headers.get_value("CoNtEnT-LeNgTh").value() == "1024");
        headers.add("Connection", "keep-alive");

        CHECK(headers.items().size() == 2);

        auto contentLength = headers.get_value("Content-Length");
        CHECK(contentLength.has_value());
        CHECK(contentLength.value() == "1024");

        auto connection = headers.get_value("Connection");
        CHECK(connection.has_value());
        CHECK(connection.value() == "keep-alive");

        CHECK(!headers.get_value("Host").has_value());
    }

    TEST_CASE(test_http_request) {
        THttpRequest request;

        request.set_method(EHttpRequestMethod::POST);
        CHECK(request.method() == EHttpRequestMethod::POST);

        request.set_target("/api/v1/trade/order");
        CHECK(request.target() == "/api/v1/trade/order");

        std::string bodyPayload = R"({"symbol": "BTCUSD", "qty": 1.5})";
        request.set_body(bodyPayload);
        CHECK(request.body() == bodyPayload);

        request.headers().add("Content-Type", "application/json");
        request.headers().add("X-Request-ID", "req-777");

        const auto& constReq = request;
        CHECK(constReq.headers().items().size() == 2);
        CHECK(constReq.headers().get_value("X-Request-ID").value() == "req-777");
    }

    TEST_CASE(test_http_response) {
        THttpResponse response;

        response.set_status(EHttpResponseStatus::NOT_FOUND);
        CHECK(response.status() == EHttpResponseStatus::NOT_FOUND);

        response.set_status(EHttpResponseStatus::OK);
        CHECK(response.status() == EHttpResponseStatus::OK);

        CHECK(response.version().major == 1);
        CHECK(response.version().minor == 1);

        response.set_body("<html><body>Success</body></html>");
        CHECK(response.body() == "<html><body>Success</body></html>");

        response.headers().add("Server", "Yandex Cloud / C++ HFT");
        response.headers().add("Content-Length", std::to_string(response.body().size()));

        CHECK(response.headers().get_value("Server").value() == "Yandex Cloud / C++ HFT");
        CHECK(response.headers().get_value("Content-Length").value() == "33");
    }

} // namespace NHttp::NTests
