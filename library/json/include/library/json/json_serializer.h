#pragma once

#include <library/json/json_value.h>

#include <ostream>

namespace NJson {

    enum class EJsonFormat : uint8_t {
        Compact = 0,
        Pretty
    };

    class TJsonSerializer {
    public:
        explicit TJsonSerializer(const TJsonValue&, std::ostream&, EJsonFormat);
        void serialize();

    private:
        
        void serialize_impl(const TJsonValue&);

        void serialize_null(const TJsonValue&);
        void serialize_integer(const TJsonValue&);
        void serialize_double(const TJsonValue&);
        void serialize_boolean(const TJsonValue&);
        void serialize_string(const TJsonValue&);
        void serialize_array(const TJsonValue&);
        void serialize_object(const TJsonValue&);
    
    private:
        const TJsonValue& json_;
        std::ostream& ostream_;
        size_t indent_;
        const EJsonFormat format_;
    };

    std::ostream& operator<<(std::ostream&, const TJsonValue&);

    void serialize(const TJsonValue&, std::ostream&, EJsonFormat);

} //namespace NJson
