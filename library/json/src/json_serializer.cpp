#include <library/json/json_types.h>
#include <library/json/json_constants.h>
#include <library/json/json_serializer.h>

#include <cassert>
#include <error.h>

namespace NJson {

    TJsonSerializer::TJsonSerializer(const TJsonValue& json, std::ostream& ostream)
        : json_(json)
        , ostream_(ostream)
    {}

    void TJsonSerializer::serialize() {
        return serialize_impl(json_);
    }

    void TJsonSerializer::serialize_impl(const TJsonValue& json) {
        if (json.is_null()) {
            serialize_null(json);
        } else if (json.is_integer()) {
            serialize_integer(json);
        } else if (json.is_double()) {
            serialize_double(json);
        } else if (json.is_boolean()) {
            serialize_boolean(json);
        } else if (json.is_string()) {
            serialize_string(json);
        } else if (json.is_array()) {
            serialize_array(json);
        } else if (json.is_object()) {
            serialize_object(json);
        } else {
            assert(false);
        }
    }

    void TJsonSerializer::serialize_null(const TJsonValue& json) {
        ostream_ << std::string("null");
    }

    void TJsonSerializer::serialize_integer(const TJsonValue& json) {
        ostream_ << std::to_string(json.get_integer());
    }

    void TJsonSerializer::serialize_double(const TJsonValue& json) {
        ostream_ << std::to_string(json.get_double());
    }

    void TJsonSerializer::serialize_boolean(const TJsonValue& json) {
        if (json.get_boolean()) {
            ostream_ << std::string("true");
        } else {
            ostream_ << std::string("false");
        }
    }

    void TJsonSerializer::serialize_string(const TJsonValue& json) {
        ostream_ << NConstants::OPEN_STRING << json.get_string() << NConstants::CLOSE_STRING;
    }

    void TJsonSerializer::serialize_array(const TJsonValue& json) {
        ostream_ << NConstants::OPEN_ARRAY;
        const TArray& array = json.get_array();
        for (size_t i = 0; i < array.size(); ++i) {
            serialize_impl(array[i]);
            if (i + 1 != array.size()) {
                ostream_ << NConstants::COMMA;
            }
        }
        ostream_ << NConstants::CLOSE_ARRAY;
    }

    void TJsonSerializer::serialize_object(const TJsonValue& json) {
        ostream_ << NConstants::OPEN_OBJECT;
        const TObject& object = json.get_object();
        {
            size_t i = 0;
            for (const auto& [str_key, json_value] : object) {
                ostream_ << NConstants::OPEN_STRING << str_key << NConstants::CLOSE_STRING << NConstants::COLON;
                serialize_impl(json_value);
                if (i + 1 != object.size()) {
                    ostream_ << NConstants::COMMA;
                }
                ++i;
            }
        }
        ostream_ << NConstants::CLOSE_OBJECT;
    }

    std::ostream& operator<<(std::ostream& ostream, const TJsonValue& json) {
        TJsonSerializer serializer(json, ostream);
        serializer.serialize();
        return ostream;
    }

} //namespace NJson
