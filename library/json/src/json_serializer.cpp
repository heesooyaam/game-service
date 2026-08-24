#include <library/json/json_serializer.h>
#include <library/json/json_types.h>

#include "json_constants.h"

#include <cassert>

namespace {

    std::string sanitize_string(std::string_view input) {
        std::string result;
        result.reserve(input.size());
        result.push_back(NJson::NConstants::OPEN_STRING);
        for (char symbol : input) {
            switch (symbol) {
                case '\"': {
                    result.push_back('\\');
                    result.push_back('"'); 
                    break;  
                }
                case '\\': {
                    result.push_back('\\');
                    result.push_back('\\');  
                    break; 
                }
                case '/': {
                    result.push_back('\\');
                    result.push_back('/'); 
                    break;  
                }
                case '\b': {
                    result.push_back('\\');
                    result.push_back('b'); 
                    break; 
                }
                case '\f': {
                    result.push_back('\\');
                    result.push_back('f'); 
                    break; 
                }
                case '\n': {
                    result.push_back('\\');
                    result.push_back('n'); 
                    break; 
                }
                case '\r': {
                    result.push_back('\\');
                    result.push_back('r'); 
                    break; 
                }
                case '\t': {
                    result.push_back('\\');
                    result.push_back('t'); 
                    break; 
                }
                default: {

                    if (static_cast<unsigned char>(symbol) < 0x20) {
                        throw NJson::NError::TJsonSerializerError("unescaped control character in string");
                    }

                    result.push_back(symbol);
                }
            }
        }
        result.push_back(NJson::NConstants::CLOSE_STRING);
        return result;
    }

    std::string sanitize_double(double value) {
        constexpr size_t buf_size = 32;
        char buf[buf_size];
        
        std::to_chars_result result = std::to_chars(buf, buf + buf_size, value);
        assert(result.ec == std::errc());
        
        std::string str(buf, result.ptr - buf);
        if (str.find('.') == std::string::npos && str.find('e') == std::string::npos) {
            str += std::string(".0");
        }

        return str;
    }

}

namespace NJson {

    TJsonSerializer::TJsonSerializer(const TJsonValue& json, std::ostream& ostream, EJsonFormat format)
        : json_(json)
        , ostream_(ostream)
        , indent_(0)
        , format_(format)
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

    void TJsonSerializer::serialize_null(const TJsonValue&) {
        ostream_ << std::string("null");
    }

    void TJsonSerializer::serialize_integer(const TJsonValue& json) {
        ostream_ << std::to_string(json.get_integer());
    }

    void TJsonSerializer::serialize_double(const TJsonValue& json) {
        ostream_ << sanitize_double(json.get_double());
    }

    void TJsonSerializer::serialize_boolean(const TJsonValue& json) {
        if (json.get_boolean()) {
            ostream_ << std::string("true");
        } else {
            ostream_ << std::string("false");
        }
    }

    void TJsonSerializer::serialize_string(const TJsonValue& json) {
        ostream_ << sanitize_string(json.get_string());
    }

    void TJsonSerializer::serialize_array(const TJsonValue& json) {
        ostream_ << NConstants::OPEN_ARRAY;
        const TArray& array = json.get_array();
        for (size_t i = 0; i < array.size(); ++i) {
            serialize_impl(array[i]);
            if (i + 1 != array.size()) {
                ostream_ << NConstants::COMMA;
                if (format_ == EJsonFormat::Pretty) {
                    ostream_ << " ";
                }
            }
        }
        ostream_ << NConstants::CLOSE_ARRAY;
    }

    void TJsonSerializer::serialize_object(const TJsonValue& json) {
        const TObject& object = json.get_object();

        if (object.size() == 0) {
            ostream_ << NConstants::OPEN_OBJECT << NConstants::CLOSE_OBJECT;
            return;
        }

        ostream_ << NConstants::OPEN_OBJECT;

        if (format_ == EJsonFormat::Pretty) {
            ostream_ << "\n";
            indent_ += 4;
        }

        {
            size_t i = 0;
            for (const auto& [str_key, json_value] : object) {
                if (format_ == EJsonFormat::Pretty) {
                    ostream_ << std::string(indent_, ' ');
                }
                
                ostream_ << sanitize_string(str_key);

                if (format_ == EJsonFormat::Pretty) {
                    ostream_ << " " << NConstants::COLON << " ";
                } else {
                    ostream_ << NConstants::COLON;
                }

                serialize_impl(json_value);
                if (i + 1 != object.size()) {
                    ostream_ << NConstants::COMMA;
                }

                if (format_ == EJsonFormat::Pretty) {
                    ostream_ << "\n";
                }

                ++i;
            }
        }

        if (format_ == EJsonFormat::Pretty) {
            indent_ -= 4;
            ostream_ << std::string(indent_, ' ');
        }

        ostream_ << NConstants::CLOSE_OBJECT;
    }

    std::ostream& operator<<(std::ostream& ostream, const TJsonValue& json) {
        TJsonSerializer serializer(json, ostream, EJsonFormat::Compact);
        serializer.serialize();
        return ostream;
    }

    void serialize(const TJsonValue& json, std::ostream& ostream, EJsonFormat format) {
        TJsonSerializer serializer(json, ostream, format);
        serializer.serialize();
    }

} //namespace NJson
