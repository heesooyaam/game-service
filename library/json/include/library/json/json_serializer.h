#pragma once

#include <library/json/json_value.h>

namespace NJson {

    template <typename TOstream>
    class TJsonSerializer {
    public:
        explicit TJsonSerializer(const TJsonValue&, TOstream&);
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
        TOstream& ostream_;
    };

    template <typename TOstream>
    TOstream& operator<<(TOstream&, const TJsonValue&);

} //namespace NJson

#define LIBRARY_JSON_SERIALIZER_H
#include <library/json/detail/json_serializer-inl.h>
#undef LIBRARY_JSON_SERIALIZER_H
