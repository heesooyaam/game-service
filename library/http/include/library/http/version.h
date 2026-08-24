#pragma once

#include <cstdint>

namespace NHttp {

    struct THttpVersion {
        uint16_t major = 1;
        uint16_t minor = 1;
        
        bool operator==(const THttpVersion& other) const noexcept;
        bool operator!=(const THttpVersion& other) const noexcept;
    };

} // namespace NHttp
