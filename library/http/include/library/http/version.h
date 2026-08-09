#pragma once

#include <cstdint>

namespace NHttp {

    struct THttpVersion {
        uint8_t major = 1;
        uint8_t minor = 1;
        
        bool operator==(const THttpVersion& other) const noexcept;
        bool operator!=(const THttpVersion& other) const noexcept;
    };

} //namespace NHttp
