#include <library/http/response/response.h>
#include <library/http/request/request.h>
#include <library/http/error.h>
#include <library/http/model/date.h>
#include <library/http/model/validate.h>

#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>

constexpr void static_check(bool value) {
    if (!value) {
        std::exit(EXIT_FAILURE);
    }
}

// void check(bool value) {
//     if (!value) {
//         std::exit(EXIT_FAILURE);
//     }
// }

#define check(value) \
    do { \
        if (!(value)) { \
            std::cerr << "[ОШИБКА ТЕСТА] Упало на строке: " << __LINE__ \
                      << " в файле: " << __FILE__ << "\n" \
                      << "Выражение: " << #value << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while(0)


namespace NHttp::NTests {

    void test_date() {
        static_check(NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT"));
        static_check(NData::is_valid_http_date("Wed, 09 Jun 2021 10:18:14 GMT"));
        static_check(NData::is_valid_http_date("Mon, 10 Aug 2026 21:25:35 GMT"));

        static_check(NData::is_valid_http_date("Tue, 01 Jan 2030 00:00:00 GMT"));
        static_check(NData::is_valid_http_date("Mon, 31 Dec 2029 23:59:59 GMT"));

        static_check(NData::is_valid_http_date("Tue, 30 Jun 2015 23:59:60 GMT"));

        static_check(NData::is_valid_http_date("Sat, 29 Feb 2020 12:00:00 GMT"));
        static_check(NData::is_valid_http_date("Tue, 29 Feb 2000 00:00:00 GMT"));

        static_check(NData::is_valid_http_date("Thu, 30 Apr 2020 15:30:00 GMT"));
        static_check(NData::is_valid_http_date("Sun, 31 May 2020 15:30:00 GMT"));
        static_check(NData::is_valid_http_date("Sat, 31 Oct 2020 23:59:59 GMT"));

        check(NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT"));
        check(NData::is_valid_http_date("Wed, 09 Jun 2021 10:18:14 GMT"));
        check(NData::is_valid_http_date("Mon, 10 Aug 2026 21:25:35 GMT"));
        check(NData::is_valid_http_date("Tue, 01 Jan 2030 00:00:00 GMT"));
        check(NData::is_valid_http_date("Mon, 31 Dec 2029 23:59:59 GMT"));
        check(NData::is_valid_http_date("Tue, 30 Jun 2015 23:59:60 GMT"));
        check(NData::is_valid_http_date("Sat, 29 Feb 2020 12:00:00 GMT"));
        check(NData::is_valid_http_date("Tue, 29 Feb 2000 00:00:00 GMT"));
        check(NData::is_valid_http_date("Thu, 30 Apr 2020 15:30:00 GMT"));
        check(NData::is_valid_http_date("Sun, 31 May 2020 15:30:00 GMT"));
        check(NData::is_valid_http_date("Sat, 31 Oct 2020 23:59:59 GMT"));

        check(!NData::is_valid_http_date(""));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 GMT "));
        check(!NData::is_valid_http_date("Sun, 6 Nov 1994 08:49:37 GMT"));

        check(!NData::is_valid_http_date("Sun 06 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun; 06 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06-Nov-1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08-49-37 GMT"));
        check(!NData::is_valid_http_date("Sun,  06 Nov 1994 08:49:37 GMT"));

        check(!NData::is_valid_http_date("Sun, 29 Feb 2021 12:00:00 GMT"));
        check(!NData::is_valid_http_date("Thu, 29 Feb 1900 12:00:00 GMT")); 
        check(!NData::is_valid_http_date("Fri, 29 Feb 2100 12:00:00 GMT"));

        check(!NData::is_valid_http_date("Fri, 31 Apr 2021 12:00:00 GMT"));
        check(!NData::is_valid_http_date("Tue, 31 Jun 2021 12:00:00 GMT"));
        check(!NData::is_valid_http_date("Wed, 31 Sep 2021 12:00:00 GMT"));
        check(!NData::is_valid_http_date("Mon, 31 Nov 2021 12:00:00 GMT"));
        check(!NData::is_valid_http_date("Sun, 30 Feb 2020 12:00:00 GMT"));

        check(!NData::is_valid_http_date("Sun, 00 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 32 Jan 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 24:00:00 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:60:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:61 GMT"));

        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 UTC"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 PST"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 +0000"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 Z"));

        check(!NData::is_valid_http_date("sun, 06 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("SUN, 06 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37 gmt"));

        check(!NData::is_valid_http_date("Foo, 06 Nov 1994 08:49:37 GMT"));
        check(!NData::is_valid_http_date("Sun, 06 Bar 1994 08:49:37 GMT"));

        check(!NData::is_valid_http_date("Sun, 06 Nov 1994 08:49:37\r\nGMT"));
        check(!NData::is_valid_http_date(std::string("Sun, 06 Nov 1994 08:49:37 ") + '\0' + std::string("MT")));
        check(!NData::is_valid_http_date(std::string("Sun, 06 Nov 1994 08:49:37 GMT") + '\0'));
        check(!NData::is_valid_http_date("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
        check(!NData::is_valid_http_date("123, 45 678 9012 34:56:78 901"));
    }

    void test_all_model_validation() {
        using namespace NHttp::NModel;

        {
            check(validate_version(THttpVersion(1, 1)));
            check(!validate_version(THttpVersion(1, 0)));
            check(!validate_version(THttpVersion(2, 0)));
            check(!validate_version(THttpVersion(0, 9)));

            check(validate_method(EHttpRequestMethod::GET));
            check(validate_method(EHttpRequestMethod::POST));
            check(validate_method(EHttpRequestMethod::PUT));
            check(validate_method(EHttpRequestMethod::PATCH));
            check(validate_method(EHttpRequestMethod::DELETE));
            check(validate_method(EHttpRequestMethod::HEAD));
            check(validate_method(EHttpRequestMethod::OPTIONS));
            check(!validate_method(EHttpRequestMethod::NOT_SET));

            check(validate_status(EHttpResponseStatus::SWITCHING_PROTOCOLS));
            check(validate_status(EHttpResponseStatus::OK));
            check(validate_status(EHttpResponseStatus::CREATED));
            check(validate_status(EHttpResponseStatus::NO_CONTENT));
            check(validate_status(EHttpResponseStatus::BAD_REQUEST));
            check(validate_status(EHttpResponseStatus::UNAUTHORIZED));
            check(validate_status(EHttpResponseStatus::FORBIDDEN));
            check(validate_status(EHttpResponseStatus::NOT_FOUND));
            check(validate_status(EHttpResponseStatus::INTERNAL_SERVER_ERROR));
            check(validate_status(EHttpResponseStatus::NOT_IMPLEMENTED));
            check(!validate_status(EHttpResponseStatus::NOT_SET));
        }

        {
            static_check(HTTP_CHAR_NAME_VALID_CHARS['a'] && HTTP_CHAR_NAME_VALID_CHARS['Z'] && HTTP_CHAR_NAME_VALID_CHARS['0']);
            static_check(HTTP_CHAR_NAME_VALID_CHARS['-'] && HTTP_CHAR_NAME_VALID_CHARS['_'] && HTTP_CHAR_NAME_VALID_CHARS['!']);
            static_check(!HTTP_CHAR_NAME_VALID_CHARS[' '] && !HTTP_CHAR_NAME_VALID_CHARS[':'] && !HTTP_CHAR_NAME_VALID_CHARS['\r']);

            check(validate_header_name_general("Content-Type"));
            check(validate_header_name_general("X-Custom_Header-123!#"));
            check(!validate_header_name_general(""));                      
            check(!validate_header_name_general("Content Type"));           
            check(!validate_header_name_general("Header:Name"));            
            check(!validate_header_name_general("Header\r\n"));           

            static_check(HTTP_CHAR_VALUE_VALID_CHARS[' '] && HTTP_CHAR_VALUE_VALID_CHARS['\t'] && HTTP_CHAR_VALUE_VALID_CHARS['a']);
            static_check(!HTTP_CHAR_VALUE_VALID_CHARS['\r'] && !HTTP_CHAR_VALUE_VALID_CHARS['\n'] && !HTTP_CHAR_VALUE_VALID_CHARS['\0']);

            check(validate_header_value_general("text/html; charset=utf-8"));
            check(validate_header_value_general("value with\tspace and tab"));
            check(validate_header_value_general(""));                       
            check(!validate_header_value_general("bad\r\nvalue"));        
            check(!validate_header_value_general("bad\nvalue"));             
            check(!validate_header_value_general(std::string_view("val\0ue", 6)));

            check(validate_headers_count(0));
            check(validate_headers_count(MAX_COUNTER_HEADERS));
            check(!validate_headers_count(MAX_COUNTER_HEADERS + 1));

            check(validate_body(""));
            check(validate_body(std::string(MAX_COUNTER_BYTES_BODY, 'a')));
            check(!validate_body(std::string(MAX_COUNTER_BYTES_BODY + 1, 'a')));
        }


        {
            check(validate_origin_form_target("/"));
            check(validate_origin_form_target("/index.html"));
            check(validate_origin_form_target("/api/v1/users?id=10&name=john%20doe"));
            check(validate_origin_form_target("/path%2Fwith%2Aescaped"));
            check(!validate_origin_form_target(""));                   
            check(!validate_origin_form_target("index.html"));           
            check(!validate_origin_form_target("/path with spaces"));     
            check(!validate_origin_form_target("/path%2"));              
            check(!validate_origin_form_target("/path%2G"));    
            check(!validate_origin_form_target(("/users#admin")));     

            {
                THttpHeaders headers;
                headers.add("Host", "example.com");
                check(validate_headers_request(headers, "", EHttpRequestMethod::GET));

                THttpHeaders no_host;
                check(!validate_headers_request(no_host, "", EHttpRequestMethod::GET));

                THttpHeaders empty_host;
                empty_host.add("Host", "");
                check(!validate_headers_request(empty_host, "", EHttpRequestMethod::GET));

                THttpHeaders dup_host;
                dup_host.add("Host", "example.com");
                dup_host.add("Host", "evil.com");
                check(!validate_headers_request(dup_host, "", EHttpRequestMethod::GET));
            }

            {
                THttpHeaders te_headers;
                te_headers.add("Host", "example.com");
                te_headers.add("Transfer-Encoding", "chunked");
                check(!validate_headers_request(te_headers, "", EHttpRequestMethod::GET));
                check(!validate_headers_request(te_headers, "", EHttpRequestMethod::HEAD));

                THttpHeaders cl_headers;
                cl_headers.add("Host", "example.com");
                cl_headers.add("Content-Length", "5");
                check(!validate_headers_request(cl_headers, "hello", EHttpRequestMethod::GET));

                THttpHeaders cl_zero;
                cl_zero.add("Host", "example.com");
                cl_zero.add("Content-Length", "0");
                check(validate_headers_request(cl_zero, "", EHttpRequestMethod::GET));
            }
        }

        {
            {
                THttpHeaders headers;
                headers.add("Host", "example.com");
                headers.add("Content-Length", "5");
                headers.add("Transfer-Encoding", "chunked");
                check(!validate_headers_request(headers, "hello", EHttpRequestMethod::POST));
            }

            {
                THttpHeaders no_framing;
                no_framing.add("Host", "example.com");
                check(!validate_headers_request(no_framing, "hello", EHttpRequestMethod::POST));
                check(validate_headers_request(no_framing, "", EHttpRequestMethod::POST));
            }

            {
                THttpHeaders h1;
                h1.add("Host", "example.com");
                h1.add("Content-Length", "5");
                check(validate_headers_request(h1, "hello", EHttpRequestMethod::POST));

                THttpHeaders h2;
                h2.add("Host", "example.com");
                h2.add("Content-Length", "10");
                check(!validate_headers_request(h2, "hello", EHttpRequestMethod::POST));

                THttpHeaders h3;
                h3.add("Host", "example.com");
                h3.add("Content-Length", "-5");
                check(!validate_headers_request(h3, "", EHttpRequestMethod::POST));

                THttpHeaders h4;
                h4.add("Host", "example.com");
                h4.add("Content-Length", "5a");
                check(!validate_headers_request(h4, "", EHttpRequestMethod::POST));

                THttpHeaders h5;
                h5.add("Host", "example.com");
                h5.add("Content-Length", "5");
                h5.add("Content-Length", "5");
                check(!validate_headers_request(h5, "hello", EHttpRequestMethod::POST));
            }

            {
                THttpHeaders h1;
                h1.add("Host", "example.com");
                h1.add("Transfer-Encoding", "chunked");
                check(!validate_headers_request(h1, "", EHttpRequestMethod::POST));

                THttpHeaders h2;
                h2.add("Host", "example.com");
                h2.add("Transfer-Encoding", "cHuNkEd");
                check(!validate_headers_request(h2, "1", EHttpRequestMethod::POST));
            }
        }

        {
            constexpr std::string_view valid_date = "Mon, 10 Aug 2026 21:25:35 GMT";

            {
                THttpHeaders headers;
                headers.add("Date", std::string(valid_date));
                headers.add("Host", "example.com");
                check(!validate_headers_response(headers, "", EHttpResponseStatus::OK));
            }

            {
                THttpHeaders h1;
                h1.add("Date", std::string(valid_date));
                check(validate_headers_response(h1, "", EHttpResponseStatus::OK));

                THttpHeaders h1_body;
                h1_body.add("Date", std::string(valid_date));
                h1_body.add("Content-Length", "5");
                check(validate_headers_response(h1_body, "hello", EHttpResponseStatus::OK));

                THttpHeaders h1_bad_body;
                h1_bad_body.add("Date", std::string(valid_date));
                check(!validate_headers_response(h1_bad_body, "hello", EHttpResponseStatus::OK));

                THttpHeaders no_date;
                check(validate_headers_response(no_date, "", EHttpResponseStatus::OK));

                THttpHeaders bad_date;
                bad_date.add("Date", "Sun, 06 Nov 1994 08:49:37 UTC"); // UTC запрещен
                check(!validate_headers_response(bad_date, "", EHttpResponseStatus::OK));

                THttpHeaders dup_date;
                dup_date.add("Date", std::string(valid_date));
                dup_date.add("Date", std::string(valid_date));
                check(!validate_headers_response(dup_date, "", EHttpResponseStatus::OK));
            }

            {
                constexpr auto STATUS = EHttpResponseStatus::SWITCHING_PROTOCOLS;
                THttpHeaders h1; 
                h1.add("Upgrade", "websocket");
                h1.add("Connection", "Upgrade");
                check(validate_headers_response(h1, "", STATUS));

                THttpHeaders h2;
                h2.add("Upgrade", "HTTP/2.0");
                h2.add("Connection", "keep-alive, uPgRaDe");
                check(validate_headers_response(h2, "", STATUS));

                THttpHeaders h3;
                check(!validate_headers_response(h3, "", STATUS));

                THttpHeaders h4;
                h4.add("Connection", "Upgrade");
                check(!validate_headers_response(h4, "", STATUS));

                THttpHeaders h5;
                h5.add("Upgrade", "websocket");
                check(!validate_headers_response(h5, "", STATUS));

                THttpHeaders h6;
                h6.add("Upgrade", "websocket");
                h6.add("Connection", "keep-alive, close");
                check(!validate_headers_response(h6, "", STATUS));

                THttpHeaders h7;
                h7.add("Upgrade", "websocket");
                h7.add("Connection", "Upgrade");
                h7.add("Content-Length", "0");
                check(!validate_headers_response(h7, "", STATUS));

                THttpHeaders h8;
                h8.add("Upgrade", "websocket");
                h8.add("Connection", "Upgrade");
                h8.add("Transfer-Encoding", "chunked");
                check(!validate_headers_response(h8, "", STATUS));
            }

            {
                THttpHeaders h1;
                h1.add("Date", std::string(valid_date));
                check(validate_headers_response(h1, "", EHttpResponseStatus::NO_CONTENT));

                THttpHeaders h2;
                h2.add("Date", std::string(valid_date));
                h2.add("Content-Length", "0");
                check(!validate_headers_response(h2, "", EHttpResponseStatus::NO_CONTENT));

                THttpHeaders h3;
                h3.add("Date", std::string(valid_date));
                h3.add("Content-Length", "5");
                check(!validate_headers_response(h3, "hello", EHttpResponseStatus::NO_CONTENT));

                THttpHeaders h4;
                h4.add("Date", std::string(valid_date));
                h4.add("Transfer-Encoding", "chunked");
                check(!validate_headers_response(h4, "", EHttpResponseStatus::NO_CONTENT));
            }

            {
                THttpHeaders h1;
                h1.add("Date", std::string(valid_date));
                h1.add("Location", "/users/42");
                check(validate_headers_response(h1, "", EHttpResponseStatus::CREATED));

                THttpHeaders h2;
                h2.add("Date", std::string(valid_date));
                h2.add("Location", "https://example.com/users/42");
                check(validate_headers_response(h2, "", EHttpResponseStatus::CREATED));

                THttpHeaders no_loc;
                no_loc.add("Date", std::string(valid_date));
                check(validate_headers_response(no_loc, "", EHttpResponseStatus::CREATED));

                THttpHeaders empty_loc;
                empty_loc.add("Date", std::string(valid_date));
                empty_loc.add("Location", "");
                check(!validate_headers_response(empty_loc, "", EHttpResponseStatus::CREATED));

                THttpHeaders dup_loc;
                dup_loc.add("Date", std::string(valid_date));
                dup_loc.add("Location", "/a");
                dup_loc.add("Location", "/b");
                check(!validate_headers_response(dup_loc, "", EHttpResponseStatus::CREATED));
            }

            {
                THttpHeaders h1;
                h1.add("Date", std::string(valid_date));
                h1.add("WWW-Authenticate", "Bearer realm=\"api\"");
                check(validate_headers_response(h1, "", EHttpResponseStatus::UNAUTHORIZED));

                THttpHeaders no_auth;
                no_auth.add("Date", std::string(valid_date));
                check(!validate_headers_response(no_auth, "", EHttpResponseStatus::UNAUTHORIZED));
            }
        }
    }

    void test_request_serialization() {

        auto verify = [](const THttpRequest& req, std::string_view expected) {
            std::ostringstream ostream;
            req.serialize(ostream);
            check(ostream.str() == expected);
        };

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::GET);
            req.set_target("/index.html");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            
            verify(req, 
                "GET /index.html HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::GET);
            req.set_target("/api/search?q=test&page=2");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "api.example.com");
            
            verify(req, 
                "GET /api/search?q=test&page=2 HTTP/1.1\r\n"
                "Host: api.example.com\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::POST);
            req.set_target("/submit");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            req.headers().add("Content-Length", "11");
            req.set_body("hello world");
            
            verify(req, 
                "POST /submit HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Content-Length: 11\r\n"
                "\r\n"
                "hello world"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::PUT);
            req.set_target("/file.txt");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            req.headers().add("Content-Length", "4");
            req.set_body("data");
            
            verify(req, 
                "PUT /file.txt HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Content-Length: 4\r\n"
                "\r\n"
                "data"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::DELETE);
            req.set_target("/resource/42");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            
            verify(req, 
                "DELETE /resource/42 HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::HEAD);
            req.set_target("/status");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            
            verify(req, 
                "HEAD /status HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::GET);
            req.set_target("/");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            req.headers().add("Accept", "text/html");
            req.headers().add("Accept", "application/json");
            
            verify(req, 
                "GET / HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Accept: text/html\r\n"
                "Accept: application/json\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::GET);
            req.set_target("/");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            req.headers().add("Cookie", "session=123");
            req.headers().add("Cookie", "theme=dark");
            
            verify(req, 
                "GET / HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "Cookie: session=123\r\n"
                "Cookie: theme=dark\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::POST);
            req.set_target("/stream");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            
            verify(req, 
                "POST /stream HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::GET);
            req.set_target("/");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("hOsT", "example.com");
            req.headers().add("x-CuStOm", "value");
            
            verify(req, 
                "GET / HTTP/1.1\r\n"
                "hOsT: example.com\r\n"
                "x-CuStOm: value\r\n"
                "\r\n"
            );
        }

        {
            THttpRequest req;
            req.set_method(EHttpRequestMethod::OPTIONS);
            req.set_target("/api");
            req.set_version(THttpVersion(1, 1));
            req.headers().add("Host", "example.com");
            
            verify(req, 
                "OPTIONS /api HTTP/1.1\r\n"
                "Host: example.com\r\n"
                "\r\n"
            );
        }
    }

    void test_response_serialization() {
        constexpr std::string_view valid_date = "Mon, 10 Aug 2026 21:25:35 GMT";

        auto verify = [](const THttpResponse& resp, std::string_view expected) {
            std::ostringstream ostream;
            resp.serialize(ostream);
            check(ostream.str() == expected);
        };

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::OK);
            resp.headers().add("Date", std::string(valid_date));
            
            verify(resp, 
                "HTTP/1.1 200 OK\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::OK);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("Content-Length", "2");
            resp.set_body("OK");
            
            verify(resp, 
                "HTTP/1.1 200 OK\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "Content-Length: 2\r\n"
                "\r\n"
                "OK"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::OK);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("Set-Cookie", "session=123; Path=/");
            resp.headers().add("Set-Cookie", "theme=dark; Secure");
            
            verify(resp, 
                "HTTP/1.1 200 OK\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "Set-Cookie: session=123; Path=/\r\n"
                "Set-Cookie: theme=dark; Secure\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::OK);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("sEt-CoOkIe", "a=1");
            resp.headers().add("SeT-cOoKiE", "b=2");
            
            verify(resp, 
                "HTTP/1.1 200 OK\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "sEt-CoOkIe: a=1\r\n"
                "SeT-cOoKiE: b=2\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::CREATED);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("Location", "/users/42");
            resp.headers().add("Content-Length", "0");
            
            verify(resp, 
                "HTTP/1.1 201 CREATED\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "Location: /users/42\r\n"
                "Content-Length: 0\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::NO_CONTENT);
            resp.headers().add("Date", std::string(valid_date));
            
            verify(resp, 
                "HTTP/1.1 204 NO CONTENT\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::SWITCHING_PROTOCOLS);
            resp.headers().add("Upgrade", "websocket");
            resp.headers().add("Connection", "Upgrade");
            
            verify(resp, 
                "HTTP/1.1 101 SWITCHING PROTOCOLS\r\n"
                "Upgrade: websocket\r\n"
                "Connection: Upgrade\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::UNAUTHORIZED);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("WWW-Authenticate", "Bearer realm=\"api\"");
            
            verify(resp, 
                "HTTP/1.1 401 UNAUTHORIZED\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "WWW-Authenticate: Bearer realm=\"api\"\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::NOT_FOUND);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("Content-Type", "text/plain");
            resp.headers().add("Content-Length", "9");
            resp.set_body("Not Found");
            
            verify(resp, 
                "HTTP/1.1 404 NOT FOUND\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 9\r\n"
                "\r\n"
                "Not Found"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::INTERNAL_SERVER_ERROR);
            resp.headers().add("Date", std::string(valid_date));
            resp.headers().add("X-Error-Code", "E1");
            resp.headers().add("X-Error-Code", "E2");
            
            verify(resp, 
                "HTTP/1.1 500 INTERNAL SERVER ERROR\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "X-Error-Code: E1\r\n"
                "X-Error-Code: E2\r\n"
                "\r\n"
            );
        }

        {
            THttpResponse resp;
            resp.set_version(THttpVersion(1, 1));
            resp.set_status(EHttpResponseStatus::OK);
            resp.headers().add("Date", std::string(valid_date));
            
            verify(resp, 
                "HTTP/1.1 200 OK\r\n"
                "Date: Mon, 10 Aug 2026 21:25:35 GMT\r\n"
                "\r\n"
            );
        }
    }

    void run_all_tests() {
        test_date();
        test_all_model_validation();
        test_request_serialization();
        test_response_serialization();
    }

} //namespace NHttp::NTests

int main() {
    NHttp::NTests::run_all_tests();
    return 0;
}
