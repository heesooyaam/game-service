#include <library/json/json_types.h>
#include <library/json/json_constants.h>
#include <library/json/json_serializer.h>

#include <cassert>
#include <error.h>

namespace NJson {

    TJsonSerializer::TJsonSerializer(const TJsonValue& json, std::ostream& ostream)
        : json_(json)
        , ostream_(ostream)
        , indent_(0)
    {}

    void TJsonSerializer::serialize() {
        return serialize_impl(json_);
    }

    void TJsonSerializer::serialize_normal_mode() {
        return serialize_impl(json_, false);
    }

    void TJsonSerializer::serialize_impl(const TJsonValue& json, bool compact) {
        if (json.is_null()) {
            serialize_null(json, compact);
        } else if (json.is_integer()) {
            serialize_integer(json, compact);
        } else if (json.is_double()) {
            serialize_double(json, compact);
        } else if (json.is_boolean()) {
            serialize_boolean(json, compact);
        } else if (json.is_string()) {
            serialize_string(json, compact);
        } else if (json.is_array()) {
            serialize_array(json, compact);
        } else if (json.is_object()) {
            serialize_object(json, compact);
        } else {
            assert(false);
        }
    }

    void TJsonSerializer::serialize_null(const TJsonValue& json, bool compact) {
        ostream_ << std::string("null");
    }

    void TJsonSerializer::serialize_integer(const TJsonValue& json, bool compact) {
        ostream_ << std::to_string(json.get_integer());
    }

    void TJsonSerializer::serialize_double(const TJsonValue& json, bool compact) {
        ostream_ << std::to_string(json.get_double());
    }

    void TJsonSerializer::serialize_boolean(const TJsonValue& json, bool compact) {
        if (json.get_boolean()) {
            ostream_ << std::string("true");
        } else {
            ostream_ << std::string("false");
        }
    }

    void TJsonSerializer::serialize_string(const TJsonValue& json, bool compact) {
        ostream_ << NConstants::OPEN_STRING << json.get_string() << NConstants::CLOSE_STRING;
    }

    void TJsonSerializer::serialize_array(const TJsonValue& json, bool compact) {
        ostream_ << NConstants::OPEN_ARRAY;
        const TArray& array = json.get_array();
        for (size_t i = 0; i < array.size(); ++i) {
            serialize_impl(array[i], compact);
            if (i + 1 != array.size()) {
                ostream_ << NConstants::COMMA;
                if (!compact) {
                    ostream_ << " ";
                }
            }
        }
        ostream_ << NConstants::CLOSE_ARRAY;
    }

    void TJsonSerializer::serialize_object(const TJsonValue& json, bool compact) {
        const TObject& object = json.get_object();

        if (object.size() == 0) {
            ostream_ << NConstants::OPEN_OBJECT << NConstants::CLOSE_OBJECT;
            return;
        }

        ostream_ << NConstants::OPEN_OBJECT;

        if (!compact) {
            ostream_ << "\n";
            indent_ += 4;
        }

        {
            size_t i = 0;
            for (const auto& [str_key, json_value] : object) {
                if (!compact) {
                    ostream_ << std::string(indent_, ' ');
                }
                
                ostream_ << NConstants::OPEN_STRING << str_key << NConstants::CLOSE_STRING;

                if (!compact) {
                    ostream_ << " " << NConstants::COLON << " ";
                } else {
                    ostream_ << NConstants::COLON;
                }

                serialize_impl(json_value, compact);
                if (i + 1 != object.size()) {
                    ostream_ << NConstants::COMMA;
                }

                if (!compact) {
                    ostream_ << "\n";
                }

                ++i;
            }
        }

        if (!compact) {
            indent_ -= 4;
            ostream_ << std::string(indent_, ' ');
        }

        ostream_ << NConstants::CLOSE_OBJECT;
    }

    std::ostream& operator<<(std::ostream& ostream, const TJsonValue& json) {
        TJsonSerializer serializer(json, ostream);

        //возможно вывод json через << нужен только для normal_mode
        serializer.serialize();
        return ostream;
    }

} //namespace NJson
