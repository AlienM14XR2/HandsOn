#ifndef EXCEPTION_H_
#define EXCEPTION_H_

class NoPoolException final : std::exception {
public:
    const char* what() const noexcept override {
        return "No Pool Objects.";
    }
};

#endif
