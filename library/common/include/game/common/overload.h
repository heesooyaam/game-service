#pragma once

namespace NCommon {
    template<class... Ts> struct TOverload : Ts... { using Ts::operator()...; };
    template<class... Ts> TOverload(Ts...) -> TOverload<Ts...>;
}
