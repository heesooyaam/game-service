#include <json/json_value.h>

#include <cassert>

using namespace json::value;

void check_global() {

    {
        TJsonValue json_value;
        assert(std::holds_alternative<std::monostate>(json_value.get_root_value()));
    }

    {
        TJsonValue json_value(false);
        assert(std::holds_alternative<TBoolean>(json_value.get_root_value()));       
    }

    {
        TArray array;
        array.emplace_back(false);
        array.emplace_back(1.4);
        array.emplace_back("hello");
        array.emplace_back(TString("world"));

        assert(std::holds_alternative<TBoolean>(array[0].get_root_value()));
        assert(std::holds_alternative<TNumber>(array[1].get_root_value()));
        assert(std::holds_alternative<TString>(array[2].get_root_value()));
        assert(std::holds_alternative<TString>(array[3].get_root_value()));

        TJsonValue json_value(array);
        assert(std::holds_alternative<TArrayPtr>(json_value.get_root_value()));  
        
        const TArrayPtr& cptr_array = std::get<TArrayPtr>(json_value.get_root_value());
        assert(cptr_array != nullptr);
        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::holds_alternative<TNumber>((*cptr_array)[1].get_root_value()));
        assert(std::holds_alternative<TString>((*cptr_array)[2].get_root_value()));
        assert(std::holds_alternative<TString>((*cptr_array)[3].get_root_value()));


        array[0] = 153.12;

        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::get<TBoolean>((*cptr_array)[0].get_root_value()) == false);


        TJsonValue second_json_value(json_value);
        assert(json_value == second_json_value);

        assert(std::holds_alternative<TArrayPtr>(second_json_value.get_root_value()));
        (*std::get<TArrayPtr>(second_json_value.get_root_value()))[0] = "hello";

        assert(json_value != second_json_value);
        assert(std::holds_alternative<TBoolean>((*cptr_array)[0].get_root_value()));
        assert(std::get<TBoolean>((*cptr_array)[0].get_root_value()) == false);


        json_value = second_json_value;
        assert(json_value == second_json_value);
    }
}


int main() { 
    check_global();

    return 0;
}
