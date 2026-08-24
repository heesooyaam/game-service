#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include <string_view>

namespace NHttp::NData {

    constexpr int parse_2digit(std::string_view s) noexcept {
        return (s[0] - '0') * 10 + (s[1] - '0');
    }

    constexpr int parse_4digit(std::string_view s) noexcept {
        return (s[0] - '0') * 1000 + (s[1] - '0') * 100 + (s[2] - '0') * 10 + (s[3] - '0');
    }

    constexpr int parse_month(std::string_view m) noexcept {
        if (m == "Jan") {
            return 1;
        } else if (m == "Feb") {
            return 2;
        } else if (m == "Mar") {
            return 3;
        } else if (m == "Apr") {
            return 4;
        } else if (m == "May") {
            return 5;
        } else if (m == "Jun") {
            return 6;
        } else if (m == "Jul") {
            return 7;
        } else if (m == "Aug") {
            return 8;
        } else if (m == "Sep") {
            return 9;
        } else if (m == "Oct") {
            return 10;
        } else if (m == "Nov") {
            return 11;
        } else if (m == "Dec") {
            return 12;
        }
        return 0;
    }

    constexpr int parse_weekday(std::string_view wkday) noexcept {
        if (wkday == "Sun") {
            return 0;
        } else if (wkday == "Mon") {
            return 1;
        } else if (wkday == "Tue") {
            return 2;
        } else if (wkday == "Wed") {
            return 3;
        } else if (wkday == "Thu") {
            return 4;
        } else if (wkday == "Fri") {
            return 5;
        } else if (wkday == "Sat") {
            return 6;
        }
        return -1;
    }

    constexpr bool is_valid_http_date(std::string_view sv) noexcept {
        if (sv.size() != 29) {
            return false;
        }

        if (sv[3] != ',' || sv[4] != ' ' || sv[7] != ' ' || sv[11] != ' ' ||
            sv[16] != ' ' || sv[19] != ':' || sv[22] != ':' || sv[25] != ' ') {
            return false;
        }

        if (sv.substr(26, 3) != "GMT") {
            return false;
        }

        const auto is_digit = [](char c) noexcept {
            return c >= '0' && c <= '9';
        };
        
        if (
            !is_digit(sv[5]) || !is_digit(sv[6]) ||   
            !is_digit(sv[12]) || !is_digit(sv[13]) ||   
            !is_digit(sv[14]) || !is_digit(sv[15]) ||
            !is_digit(sv[17]) || !is_digit(sv[18]) ||  
            !is_digit(sv[20]) || !is_digit(sv[21]) ||   
            !is_digit(sv[23]) || !is_digit(sv[24])
        ) {
            return false;
        }

        const int expected_wd = NHttp::NData::parse_weekday(sv.substr(0, 3));
        if (expected_wd == -1) {
            return false;
        }

        const int day    = NHttp::NData::parse_2digit(sv.substr(5, 2));
        const int month  = NHttp::NData::parse_month(sv.substr(8, 3));
        const int year   = NHttp::NData::parse_4digit(sv.substr(12, 4));
        const int hour   = NHttp::NData::parse_2digit(sv.substr(17, 2));
        const int minute = NHttp::NData::parse_2digit(sv.substr(20, 2));
        const int second = NHttp::NData::parse_2digit(sv.substr(23, 2));

        if (month == 0 || year < 1900) {
            return false;
        }

        if (hour > 23 || minute > 59 || second > 60) {
            return false;
        }

        std::chrono::year_month_day ymd{
            std::chrono::year(year),
            std::chrono::month(month),
            std::chrono::day(day)
        };

        if (!ymd.ok()) {
            return false;
        }

        std::chrono::weekday real_wd{std::chrono::sys_days{ymd}};
        if (static_cast<int>(real_wd.c_encoding()) != expected_wd) {
            return false;
        }

        return true;
    }

    inline std::string get_current_http_date() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        
        std::tm now_tm{};
        gmtime_r(&now_c, &now_tm);

        std::stringstream ss;
        ss.imbue(std::locale::classic());
        ss << std::put_time(&now_tm, "%a, %d %b %Y %H:%M:%S GMT");
        return ss.str();
    }

} // namespace NHttp::NData
