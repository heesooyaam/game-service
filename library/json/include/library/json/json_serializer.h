#pragma once

#include <library/json/json_value.h>

#include <ostream>

namespace NJson {

    class TJsonSerializer {
    public:
        explicit TJsonSerializer(const TJsonValue&, std::ostream&);
        void serialize();
        void serialize_normal_mode();

    private:
        
        void serialize_impl(const TJsonValue&, bool compact = true);

        void serialize_null(const TJsonValue&, bool);
        void serialize_integer(const TJsonValue&, bool);
        void serialize_double(const TJsonValue&, bool);
        void serialize_boolean(const TJsonValue&, bool);
        void serialize_string(const TJsonValue&, bool);
        void serialize_array(const TJsonValue&, bool);
        void serialize_object(const TJsonValue&, bool);
    
    private:
        const TJsonValue& json_;
        std::ostream& ostream_;
        size_t indent_;
    };

    std::ostream& operator<<(std::ostream&, const TJsonValue&);

} //namespace NJson
