#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/http/response/response.h>
#include <library/http/request/request.h>
#include <library/http/error.h>
#include <library/http/model/date.h>
#include <library/http/model/validate.h>

#include <string>
#include <string_view>
#include <sstream>

namespace NHttp::NTests {

    TEST_CASE(test_date_constexpr) {
        STATIC_CHECK(NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Wed, 09 Jun 2021 10:18:14 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Mon, 10 Aug 2026 21:25:35 GMT"));

        STATIC_CHECK(NData::is_valid_http_date("Tue, 01 Jan 2030 00:00:00 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Mon, 31 Dec 2029 23:59:59 GMT"));

        STATIC_CHECK(NData::is_valid_http_date("Tue, 30 Jun 2015 23:59:60 GMT"));

        STATIC_CHECK(NData::is_valid_http_date("Sat, 29 Feb 2020 12:00:00 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Tue, 29 Feb 2000 00:00:00 GMT"));

        STATIC_CHECK(NData::is_valid_http_date("Thu, 30 Apr 2020 15:30:00 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Sun, 31 May 2020 15:30:00 GMT"));
        STATIC_CHECK(NData::is_valid_http_date("Sat, 31 Oct 2020 23:59:59 GMT"));
    }

    TEST_CASE(test_date_runtime) {
        CHECK(NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT"));
        CHECK(NData::is_valid_http_date("Wed, 09 Jun 2021 10:18:14 GMT"));
        CHECK(NData::is_valid_http_date("Mon, 10 Aug 2026 21:25:35 GMT"));
        CHECK(NData::is_valid_http_date("Tue, 01 Jan 2030 00:00:00 GMT"));
        CHECK(NData::is_valid_http_date("Mon, 31 Dec 2029 23:59:59 GMT"));
        CHECK(NData::is_valid_http_date("Tue, 30 Jun 2015 23:59:60 GMT"));
        CHECK(NData::is_valid_http_date("Sat, 29 Feb 2020 12:00:00 GMT"));
        CHECK(NData::is_valid_http_date("Tue, 29 Feb 2000 00:00:00 GMT"));
        CHECK(NData::is_valid_http_date("Thu, 30 Apr 2020 15:30:00 GMT"));
        CHECK(NData::is_valid_http_date("Sun, 31 May 2020 15:30:00 GMT"));
        CHECK(NData::is_valid_http_date("Sat, 31 Oct 2020 23:59:59 GMT"));

        CHECK(!NData::is_valid_http_date(""));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT "));
        CHECK(!NData::is_valid_http_date("Sun, 6 Nov 1994 08:49:37 GMT"));

        CHECK(!NData::is_valid_http_date("Sun 06 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun; 06 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06-Nov-1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08-49-37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun,  06 Nov 1994 08:49:37 GMT"));

        CHECK(!NData::is_valid_http_date("Sun, 29 Feb 2021 12:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Thu, 29 Feb 1900 12:00:00 GMT")); 
        CHECK(!NData::is_valid_http_date("Fri, 29 Feb 2100 12:00:00 GMT"));

        CHECK(!NData::is_valid_http_date("Fri, 31 Apr 2021 12:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Tue, 31 Jun 2021 12:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Wed, 31 Sep 2021 12:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Mon, 31 Nov 2021 12:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 30 Feb 2020 12:00:00 GMT"));

        CHECK(!NData::is_valid_http_date("Sun, 00 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 32 Jan 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 24:00:00 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:60:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:61 GMT"));

        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 UTC"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 PST"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 +0000"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 Z"));

        CHECK(!NData::is_valid_http_date("sun, 06 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("SUN, 06 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 gmt"));

        CHECK(!NData::is_valid_http_date("Foo, 06 Nov 1994 08:49:37 GMT"));
        CHECK(!NData::is_valid_http_date("Sun, 06 Bar 1994 08:49:37 GMT"));

        CHECK(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37\r\nGMT"));
        CHECK(!NData::is_valid_http_date(std::string("Sun, 06 Nov 1994 08:49:37 ") + '\0' + std::string("MT")));
        CHECK(!NData::is_valid_http_date(std::string("Sun, 06 Nov 1994 08:49:37 GMT") + '\0'));
        CHECK(!NData::is_valid_http_date("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
        CHECK(!NData::is_valid_http_date("123, 45 678 9012 34:56:78 901"));
    }

    TEST_CASE(test_model_validation_enums) {
        using namespace NHttp::NModel;

        CHECK(validate_version(THttpVersion(1, 1)));
        CHECK(!validate_version(THttpVersion(1, 0)));
        CHECK(!validate_version(THttpVersion(2, 0)));
        CHECK(!validate_version(THttpVersion(0, 9)));

        CHECK(validate_method(EHttpRequestMethod::GET));
        CHECK(validate_method(EHttpRequestMethod::POST));
        CHECK(validate_method(EHttpRequestMethod::PUT));
        CHECK(validate_method(EHttpRequestMethod::PATCH));
        CHECK(validate_method(EHttpRequestMethod::DELETE));
        CHECK(validate_method(EHttpRequestMethod::HEAD));
        CHECK(validate_method(EHttpRequestMethod::OPTIONS));
        CHECK(!validate_method(EHttpRequestMethod::NOT_SET));

        CHECK(validate_status(EHttpResponseStatus::SWITCHING_PROTOCOLS));
        CHECK(validate_status(EHttpResponseStatus::OK));
        CHECK(validate_status(EHttpResponseStatus::CREATED));
        CHECK(validate_status(EHttpResponseStatus::NO_CONTENT));
        CHECK(validate_status(EHttpResponseStatus::BAD_REQUEST));
        CHECK(validate_status(EHttpResponseStatus::UNAUTHORIZED));
        CHECK(validate_status(EHttpResponseStatus::FORBIDDEN));
        CHECK(validate_status(EHttpResponseStatus::NOT_FOUND));
        CHECK(validate_status(EHttpResponseStatus::INTERNAL_SERVER_ERROR));
        CHECK(validate_status(EHttpResponseStatus::NOT_IMPLEMENTED));
        CHECK(!validate_status(EHttpResponseStatus::NOT_SET));
    }

    TEST_CASE(test_model_validation_headers_general) {
        using namespace NHttp::NModel;

        STATIC_CHECK(HTTP_NAME_VALID_CHARS['a'] && HTTP_NAME_VALID_CHARS['Z'] && HTTP_NAME_VALID_CHARS['0']);
        STATIC_CHECK(HTTP_NAME_VALID_CHARS['-'] && HTTP_NAME_VALID_CHARS['_'] && HTTP_NAME_VALID_CHARS['!']);
        STATIC_CHECK(!HTTP_NAME_VALID_CHARS[' '] && !HTTP_NAME_VALID_CHARS[':'] && !HTTP_NAME_VALID_CHARS['\r']);

        CHECK(validate_header_name_general("Content-Type"));
        CHECK(validate_header_name_general("X-Custom_Header-123!#"));
        CHECK(!validate_header_name_general(""));                      
        CHECK(!validate_header_name_general("Content Type"));          
        CHECK(!validate_header_name_general("Header:Name"));            
        CHECK(!validate_header_name_general("Header\r\n"));           

        STATIC_CHECK(HTTP_VALUE_VALID_CHARS[' '] && HTTP_VALUE_VALID_CHARS['\t'] && HTTP_VALUE_VALID_CHARS['a']);
        STATIC_CHECK(!HTTP_VALUE_VALID_CHARS['\r'] && !HTTP_VALUE_VALID_CHARS['\n'] && !HTTP_VALUE_VALID_CHARS['\0']);

        CHECK(validate_header_value_general("text/html; charset=utf-8"));
        CHECK(validate_header_value_general("value with\tspace and tab"));
        CHECK(validate_header_value_general(""));                       
        CHECK(!validate_header_value_general("bad\r\nvalue"));        
        CHECK(!validate_header_value_general("bad\nvalue"));            
        CHECK(!validate_header_value_general(std::string_view("val\0ue", 6)));

        CHECK(validate_headers_count(0));
        CHECK(validate_headers_count(MAX_COUNTER_HEADERS));
        CHECK(!validate_headers_count(MAX_COUNTER_HEADERS + 1));

        CHECK(validate_body(""));
        CHECK(validate_body(std::string(MAX_COUNTER_BYTES_BODY, 'a')));
        CHECK(!validate_body(std::string(MAX_COUNTER_BYTES_BODY + 1, 'a')));
    }

    TEST_CASE(test_model_validation_request) {
        using namespace NHttp::NModel;

        CHECK(validate_origin_form_target("/"));
        CHECK(validate_origin_form_target("/index.html"));
        CHECK(validate_origin_form_target("/api/v1/users?id=10&name=john%20doe"));
        CHECK(validate_origin_form_target("/path%2Fwith%2Aescaped"));
        CHECK(!validate_origin_form_target(""));                   
        CHECK(!validate_origin_form_target("index.html"));           
        CHECK(!validate_origin_form_target("/path with spaces"));     
        CHECK(!validate_origin_form_target("/path%2"));              
        CHECK(!validate_origin_form_target("/path%2G"));    
        CHECK(!validate_origin_form_target(("/users#admin")));     
        CHECK(!validate_origin_form_target(("/[x]")));     

        THttpHeaders headers;
        headers.add("Host", "example.com");
        CHECK(validate_headers_request(headers, "", EHttpRequestMethod::GET));

        THttpHeaders no_host;
        CHECK(!validate_headers_request(no_host, "", EHttpRequestMethod::GET));

        THttpHeaders empty_host;
        empty_host.add("Host", "");
        CHECK(!validate_headers_request(empty_host, "", EHttpRequestMethod::GET));

        THttpHeaders dup_host;
        dup_host.add("Host", "example.com");
        dup_host.add("Host", "evil.com");
        CHECK(!validate_headers_request(dup_host, "", EHttpRequestMethod::GET));

        THttpHeaders te_headers;
        te_headers.add("Host", "example.com");
        te_headers.add("Transfer-Encoding", "chunked");
        CHECK(!validate_headers_request(te_headers, "", EHttpRequestMethod::GET));
        CHECK(!validate_headers_request(te_headers, "", EHttpRequestMethod::HEAD));

        THttpHeaders cl_headers;
        cl_headers.add("Host", "example.com");
        cl_headers.add("Content-Length", "5");
        CHECK(!validate_headers_request(cl_headers, "hello", EHttpRequestMethod::GET));

        THttpHeaders cl_zero;
        cl_zero.add("Host", "example.com");
        cl_zero.add("Content-Length", "0");
        CHECK(validate_headers_request(cl_zero, "", EHttpRequestMethod::GET));

        THttpHeaders post_both;
        post_both.add("Host", "example.com");
        post_both.add("Content-Length", "5");
        post_both.add("Transfer-Encoding", "chunked");
        CHECK(!validate_headers_request(post_both, "hello", EHttpRequestMethod::POST));

        THttpHeaders no_framing;
        no_framing.add("Host", "example.com");
        CHECK(!validate_headers_request(no_framing, "hello", EHttpRequestMethod::POST));
        CHECK(validate_headers_request(no_framing, "", EHttpRequestMethod::POST));

        THttpHeaders h1;
        h1.add("Host", "example.com");
        h1.add("Content-Length", "5");
        CHECK(validate_headers_request(h1, "hello", EHttpRequestMethod::POST));

        THttpHeaders h2;
        h2.add("Host", "example.com");
        h2.add("Content-Length", "10");
        CHECK(!validate_headers_request(h2, "hello", EHttpRequestMethod::POST));

        THttpHeaders h3;
        h3.add("Host", "example.com");
        h3.add("Content-Length", "-5");
        CHECK(!validate_headers_request(h3, "", EHttpRequestMethod::POST));

        THttpHeaders h4;
        h4.add("Host", "example.com");
        h4.add("Content-Length", "5a");
        CHECK(!validate_headers_request(h4, "", EHttpRequestMethod::POST));

        THttpHeaders h5;
        h5.add("Host", "example.com");
        h5.add("Content-Length", "5");
        h5.add("Content-Length", "5");
        CHECK(!validate_headers_request(h5, "hello", EHttpRequestMethod::POST));

        THttpHeaders h6;
        h6.add("Host", "example.com");
        h6.add("Transfer-Encoding", "chunked");
        CHECK(!validate_headers_request(h6, "", EHttpRequestMethod::POST));

        THttpHeaders h7;
        h7.add("Host", "example.com");
        h7.add("Transfer-Encoding", "cHuNkEd");
        CHECK(!validate_headers_request(h7, "1", EHttpRequestMethod::POST));
    }

    TEST_CASE(test_model_validation_response) {
        using namespace NHttp::NModel;
        constexpr std::string_view valid_date = "Mon, 10 Aug 2026 21:25:35 GMT";

        THttpHeaders headers;
        headers.add("Date", std::string(valid_date));
        headers.add("Host", "example.com");
        CHECK(!validate_headers_response(headers, "", EHttpResponseStatus::OK));

        THttpHeaders h1;
        h1.add("Date", std::string(valid_date));
        CHECK(validate_headers_response(h1, "", EHttpResponseStatus::OK));

        THttpHeaders h1_body;
        h1_body.add("Date", std::string(valid_date));
        h1_body.add("Content-Length", "5");
        CHECK(validate_headers_response(h1_body, "hello", EHttpResponseStatus::OK));

        THttpHeaders h1_bad_body;
        h1_bad_body.add("Date", std::string(valid_date));
        CHECK(!validate_headers_response(h1_bad_body, "hello", EHttpResponseStatus::OK));

        THttpHeaders no_date;
        CHECK(validate_headers_response(no_date, "", EHttpResponseStatus::OK));

        THttpHeaders bad_date;
        bad_date.add("Date", "Sun, 06 Nov 1994 08:49:37 UTC"); 
        CHECK(!validate_headers_response(bad_date, "", EHttpResponseStatus::OK));

        THttpHeaders dup_date;
        dup_date.add("Date", std::string(valid_date));
        dup_date.add("Date", std::string(valid_date));
        CHECK(!validate_headers_response(dup_date, "", EHttpResponseStatus::OK));

        constexpr auto STATUS_UPGRADE = EHttpResponseStatus::SWITCHING_PROTOCOLS;
        THttpHeaders u1; 
        u1.add("Upgrade", "websocket");
        u1.add("Connection", "Upgrade");
        CHECK(validate_headers_response(u1, "", STATUS_UPGRADE));

        THttpHeaders u2;
        u2.add("Upgrade", "HTTP/2.0");
        u2.add("Connection", "keep-alive, uPgRaDe");
        CHECK(validate_headers_response(u2, "", STATUS_UPGRADE));

        THttpHeaders u3;
        CHECK(!validate_headers_response(u3, "", STATUS_UPGRADE));

        THttpHeaders u4;
        u4.add("Connection", "Upgrade");
        CHECK(!validate_headers_response(u4, "", STATUS_UPGRADE));

        THttpHeaders u5;
        u5.add("Upgrade", "websocket");
        CHECK(!validate_headers_response(u5, "", STATUS_UPGRADE));

        THttpHeaders u6;
        u6.add("Upgrade", "websocket");
        u6.add("Connection", "keep-alive, close");
        CHECK(!validate_headers_response(u6, "", STATUS_UPGRADE));

        THttpHeaders u7;
        u7.add("Upgrade", "websocket");
        u7.add("Connection", "Upgrade");
        u7.add("Content-Length", "0");
        CHECK(!validate_headers_response(u7, "", STATUS_UPGRADE));

        THttpHeaders u8;
        u8.add("Upgrade", "websocket");
        u8.add("Connection", "Upgrade");
        u8.add("Transfer-Encoding", "chunked");
        CHECK(!validate_headers_response(u8, "", STATUS_UPGRADE));

        THttpHeaders nc1;
        nc1.add("Date", std::string(valid_date));
        CHECK(validate_headers_response(nc1, "", EHttpResponseStatus::NO_CONTENT));

        THttpHeaders nc2;
        nc2.add("Date", std::string(valid_date));
        nc2.add("Content-Length", "0");
        CHECK(!validate_headers_response(nc2, "", EHttpResponseStatus::NO_CONTENT));

        THttpHeaders nc3;
        nc3.add("Date", std::string(valid_date));
        nc3.add("Content-Length", "5");
        CHECK(!validate_headers_response(nc3, "hello", EHttpResponseStatus::NO_CONTENT));

        THttpHeaders nc4;
        nc4.add("Date", std::string(valid_date));
        nc4.add("Transfer-Encoding", "chunked");
        CHECK(!validate_headers_response(nc4, "", EHttpResponseStatus::NO_CONTENT));

        THttpHeaders c1;
        c1.add("Date", std::string(valid_date));
        c1.add("Location", "/users/42");
        CHECK(validate_headers_response(c1, "", EHttpResponseStatus::CREATED));

        THttpHeaders c2;
        c2.add("Date", std::string(valid_date));
        c2.add("Location", "https://example.com/users/42");
        CHECK(validate_headers_response(c2, "", EHttpResponseStatus::CREATED));

        THttpHeaders no_loc;
        no_loc.add("Date", std::string(valid_date));
        CHECK(validate_headers_response(no_loc, "", EHttpResponseStatus::CREATED));

        THttpHeaders empty_loc;
        empty_loc.add("Date", std::string(valid_date));
        empty_loc.add("Location", "");
        CHECK(!validate_headers_response(empty_loc, "", EHttpResponseStatus::CREATED));

        THttpHeaders dup_loc;
        dup_loc.add("Date", std::string(valid_date));
        dup_loc.add("Location", "/a");
        dup_loc.add("Location", "/b");
        CHECK(!validate_headers_response(dup_loc, "", EHttpResponseStatus::CREATED));

        THttpHeaders auth1;
        auth1.add("Date", std::string(valid_date));
        auth1.add("WWW-Authenticate", "Bearer realm=\"api\"");
        CHECK(validate_headers_response(auth1, "", EHttpResponseStatus::UNAUTHORIZED));

        THttpHeaders no_auth;
        no_auth.add("Date", std::string(valid_date));
        CHECK(!validate_headers_response(no_auth, "", EHttpResponseStatus::UNAUTHORIZED));
    }

    TEST_CASE(test_request_serialization) {
        auto verify = [](const THttpRequest& req, std::string_view expected) {
            std::ostringstream ostream;
            req.serialize(ostream);
            CHECK_EQ(ostream.str(), expected);
        };

        THttpRequest req1;
        req1.set_method(EHttpRequestMethod::GET);
        req1.set_target("/index.html");
        req1.set_version(THttpVersion(1, 1));
        req1.headers().add("Host", "example.com");
        verify(req1, 
            "GET /index.html HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n"
        );

        THttpRequest req2;
        req2.set_method(EHttpRequestMethod::GET);
        req2.set_target("/api/search?q=test&page=2");
        req2.set_version(THttpVersion(1, 1));
        req2.headers().add("Host", "api.example.com");
        verify(req2, 
            "GET /api/search?q=test&page=2 HTTP/1.1\r\n"
            "Host: api.example.com\r\n"
            "\r\n"
        );

        THttpRequest req3;
        req3.set_method(EHttpRequestMethod::POST);
        req3.set_target("/submit");
        req3.set_version(THttpVersion(1, 1));
        req3.headers().add("Host", "example.com");
        req3.headers().add("Content-Length", "11");
        req3.set_body("hello world");
        verify(req3, 
            "POST /submit HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Content-Length: 11\r\n"
            "\r\n"
            "hello world"
        );

        THttpRequest req4;
        req4.set_method(EHttpRequestMethod::PUT);
        req4.set_target("/file.txt");
        req4.set_version(THttpVersion(1, 1));
        req4.headers().add("Host", "example.com");
        req4.headers().add("Content-Length", "4");
        req4.set_body("data");
        verify(req4, 
            "PUT /file.txt HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Content-Length: 4\r\n"
            "\r\n"
            "data"
        );

        THttpRequest req5;
        req5.set_method(EHttpRequestMethod::DELETE);
        req5.set_target("/resource/42");
        req5.set_version(THttpVersion(1, 1));
        req5.headers().add("Host", "example.com");
        verify(req5, 
            "DELETE /resource/42 HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n"
        );

        THttpRequest req6;
        req6.set_method(EHttpRequestMethod::HEAD);
        req6.set_target("/status");
        req6.set_version(THttpVersion(1, 1));
        req6.headers().add("Host", "example.com");
        verify(req6, 
            "HEAD /status HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n"
        );

        THttpRequest req7;
        req7.set_method(EHttpRequestMethod::GET);
        req7.set_target("/");
        req7.set_version(THttpVersion(1, 1));
        req7.headers().add("Host", "example.com");
        req7.headers().add("Accept", "text/html");
        req7.headers().add("Accept", "application/json");
        verify(req7, 
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Accept: text/html\r\n"
            "Accept: application/json\r\n"
            "\r\n"
        );

        THttpRequest req8;
        req8.set_method(EHttpRequestMethod::GET);
        req8.set_target("/");
        req8.set_version(THttpVersion(1, 1));
        req8.headers().add("Host", "example.com");
        req8.headers().add("Cookie", "session=123");
        req8.headers().add("Cookie", "theme=dark");
        verify(req8, 
            "GET / HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "Cookie: session=123\r\n"
            "Cookie: theme=dark\r\n"
            "\r\n"
        );

        THttpRequest req9;
        req9.set_method(EHttpRequestMethod::POST);
        req9.set_target("/stream");
        req9.set_version(THttpVersion(1, 1));
        req9.headers().add("Host", "example.com");
        verify(req9, 
            "POST /stream HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n"
        );

        THttpRequest req10;
        req10.set_method(EHttpRequestMethod::GET);
        req10.set_target("/");
        req10.set_version(THttpVersion(1, 1));
        req10.headers().add("hOsT", "example.com");
        req10.headers().add("x-CuStOm", "value");
        verify(req10, 
            "GET / HTTP/1.1\r\n"
            "hOsT: example.com\r\n"
            "x-CuStOm: value\r\n"
            "\r\n"
        );

        THttpRequest req11;
        req11.set_method(EHttpRequestMethod::OPTIONS);
        req11.set_target("/api");
        req11.set_version(THttpVersion(1, 1));
        req11.headers().add("Host", "example.com");
        verify(req11, 
            "OPTIONS /api HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "\r\n"
        );
    }

    TEST_CASE(test_response_serialization) {
        constexpr std::string_view valid_date = "Mon, 10 Aug 2026 21:25:35 GMT";

        auto verify = [](const THttpResponse& resp, std::string_view expected) {
            std::ostringstream ostream;
            resp.serialize(ostream);
            CHECK_EQ(ostream.str(), expected);
        };

        THttpResponse resp1;
        resp1.set_version(THttpVersion(1, 1));
        resp1.set_status(EHttpResponseStatus::OK);
        resp1.headers().add("Date", std::string(valid_date));
        verify(resp1, 
            "HTTP/1.1 200 OK\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "\r\n"
        );

        THttpResponse resp2;
        resp2.set_version(THttpVersion(1, 1));
        resp2.set_status(EHttpResponseStatus::OK);
        resp2.headers().add("Date", std::string(valid_date));
        resp2.headers().add("Content-Length", "2");
        resp2.set_body("OK");
        verify(resp2, 
            "HTTP/1.1 200 OK\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "Content-Length: 2\r\n"
            "\r\n"
            "OK"
        );

        THttpResponse resp3;
        resp3.set_version(THttpVersion(1, 1));
        resp3.set_status(EHttpResponseStatus::OK);
        resp3.headers().add("Date", std::string(valid_date));
        resp3.headers().add("Set-Cookie", "session=123; Path=/");
        resp3.headers().add("Set-Cookie", "theme=dark; Secure");
        verify(resp3, 
            "HTTP/1.1 200 OK\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "Set-Cookie: session=123; Path=/\r\n"
            "Set-Cookie: theme=dark; Secure\r\n"
            "\r\n"
        );

        THttpResponse resp4;
        resp4.set_version(THttpVersion(1, 1));
        resp4.set_status(EHttpResponseStatus::OK);
        resp4.headers().add("Date", std::string(valid_date));
        resp4.headers().add("sEt-CoOkIe", "a=1");
        resp4.headers().add("SeT-cOoKiE", "b=2");
        verify(resp4, 
            "HTTP/1.1 200 OK\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "sEt-CoOkIe: a=1\r\n"
            "SeT-cOoKiE: b=2\r\n"
            "\r\n"
        );

        THttpResponse resp5;
        resp5.set_version(THttpVersion(1, 1));
        resp5.set_status(EHttpResponseStatus::CREATED);
        resp5.headers().add("Date", std::string(valid_date));
        resp5.headers().add("Location", "/users/42");
        resp5.headers().add("Content-Length", "0");
        verify(resp5, 
            "HTTP/1.1 201 CREATED\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "Location: /users/42\r\n"
            "Content-Length: 0\r\n"
            "\r\n"
        );

        THttpResponse resp6;
        resp6.set_version(THttpVersion(1, 1));
        resp6.set_status(EHttpResponseStatus::NO_CONTENT);
        resp6.headers().add("Date", std::string(valid_date));
        verify(resp6, 
            "HTTP/1.1 204 NO CONTENT\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "\r\n"
        );

        THttpResponse resp7;
        resp7.set_version(THttpVersion(1, 1));
        resp7.set_status(EHttpResponseStatus::SWITCHING_PROTOCOLS);
        resp7.headers().add("Upgrade", "websocket");
        resp7.headers().add("Connection", "Upgrade");
        verify(resp7, 
            "HTTP/1.1 101 SWITCHING PROTOCOLS\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "\r\n"
        );

        THttpResponse resp8;
        resp8.set_version(THttpVersion(1, 1));
        resp8.set_status(EHttpResponseStatus::UNAUTHORIZED);
        resp8.headers().add("Date", std::string(valid_date));
        resp8.headers().add("WWW-Authenticate", "Bearer realm=\"api\"");
        verify(resp8, 
            "HTTP/1.1 401 UNAUTHORIZED\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "WWW-Authenticate: Bearer realm=\"api\"\r\n"
            "\r\n"
        );

        THttpResponse resp9;
        resp9.set_version(THttpVersion(1, 1));
        resp9.set_status(EHttpResponseStatus::NOT_FOUND);
        resp9.headers().add("Date", std::string(valid_date));
        resp9.headers().add("Content-Type", "text/plain");
        resp9.headers().add("Content-Length", "9");
        resp9.set_body("Not Found");
        verify(resp9, 
            "HTTP/1.1 404 NOT FOUND\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 9\r\n"
            "\r\n"
            "Not Found"
        );

        THttpResponse resp10;
        resp10.set_version(THttpVersion(1, 1));
        resp10.set_status(EHttpResponseStatus::INTERNAL_SERVER_ERROR);
        resp10.headers().add("Date", std::string(valid_date));
        resp10.headers().add("X-Error-Code", "E1");
        resp10.headers().add("X-Error-Code", "E2");
        verify(resp10, 
            "HTTP/1.1 500 INTERNAL SERVER ERROR\r\n"
            "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
            "X-Error-Code: E1\r\n"
            "X-Error-Code: E2\r\n"
            "\r\n"
        );
    }

} // namespace NHttp::NTests
