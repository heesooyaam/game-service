#include <cassert>
#include <integer/sum/heart.h>


int main() {
    assert(calculator::integer::sum::heart(1, 2) == 3);
    assert(calculator::integer::sum::heart(1, 3) == 4);
    assert(calculator::integer::sum::heart(67, 0) == 67);

    return 0;
}