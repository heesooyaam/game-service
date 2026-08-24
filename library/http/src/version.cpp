#include <library/http/version.h>

namespace NHttp {

    bool THttpVersion::operator==(const THttpVersion& other) const noexcept {
        return major == other.major && minor == other.minor;
    }
     
    bool THttpVersion::operator!=(const THttpVersion& other) const noexcept {
        return !(*this == other);
    }

} // namespace NHttp
