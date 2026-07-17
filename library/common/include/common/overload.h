#pragma once

namespace NCommon {

    template<class... Ts> 
    struct TOverloaded : Ts... { 
        using Ts::operator()...; 
    };

    template<class... Ts> 
    TOverloaded(Ts...) -> TOverloaded<Ts...>;

} //namespace NCommon
