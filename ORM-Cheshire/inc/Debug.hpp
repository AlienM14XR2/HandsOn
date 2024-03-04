#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

#endif
