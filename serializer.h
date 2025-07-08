#pragma once

#include <ostream>
#include <istream>
#include <string>
#include <cstdint>

enum class Error {
    NoError,
    CorruptedArchive
};

class Serializer {
    static constexpr char Separator = ' ';
public:
    explicit Serializer(std::ostream& out)
        : out_(out) {}

    template <class T>
    Error save(T& object) {
        return object.serialize(*this);
    }

    template <class... Args>
    Error operator()(Args... args) {
        return process(std::forward<Args>(args)...);
    }

private:
    // variadic unpacking
    template <class T, class... Args>
    Error process(T&& val, Args&&... args) {
        auto err = process(std::forward<T>(val));
        if (err != Error::NoError) return err;
        out_ << Separator;
        return process(std::forward<Args>(args)...);
    }

    // last one
    Error process(uint64_t v) {
        out_ << v;
        return Error::NoError;
    }

    Error process(bool b) {
        out_ << (b ? "true" : "false");
        return Error::NoError;
    }

    std::ostream& out_;
};

class Deserializer {
public:
    explicit Deserializer(std::istream& in)
        : in_(in) {}

    template <class T>
    Error load(T& object) {
        return object.serialize(*this);
    }

    template <class... Args>
    Error operator()(Args&... args) {
        return process(args...);
    }

private:
    template <class T, class... Args>
    Error process(T& val, Args&... args) {
        auto err = process(val);
        if (err != Error::NoError) return err;
        return process(args...);
    }

    // last one
    Error process(uint64_t& v) {
        std::string token;
        if (!(in_ >> token)) return Error::CorruptedArchive;
        // must be all digits
        for (char c : token)
            if (!std::isdigit(c))
                return Error::CorruptedArchive;
        v = std::stoull(token);
        return Error::NoError;
    }

    Error process(bool& b) {
        std::string token;
        if (!(in_ >> token)) return Error::CorruptedArchive;
        if (token == "true")  b = true;
        else if (token == "false") b = false;
        else return Error::CorruptedArchive;
        return Error::NoError;
    }

    std::istream& in_;
};