#pragma once
#include <bits/stdc++.h>
#include <source_location>
// #include <sstream>
// #include <iostream>

namespace details { //放在一个子命名空间，用户使用logger并不需要关心这里面的东西
template <class T>
struct with_source_location {
private:
    T fmt;
    std::source_location loc;

public:
    template <class U> 
    with_source_location(U &&fmt, std::source_location loc = std::source_location::current())
    : fmt(std::forward<U>(fmt)), loc(std::move(loc)) {}
    constexpr T const &format() const { return fmt; }
    constexpr std::source_location const &location() const { return loc; }
};

//因为gcc 11都还在不支持直接用format，所以用snprintf()来实现类似format的功能
template<typename... Args>
std::string string_format(const std::string& format, Args... args) {
    //snprintf类似于printf,但他不会把结果输出，而是写入数组里。
    //第一个参数传入nullptr，表示并不真的把结果写入数组，返回值是需要的存储长度
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1; //获取需要的长度，加1是为了存储最后的0
    std::unique_ptr<char[]> buf(new char[size]);    //buf只是临时用一下，因此用unique_ptr就行
    //把buf的首地址和需要的长度传入，这下buf里就是得到的结果了。
    snprintf(buf.get(), size, format.c_str(), args...); 
    return std::string(buf.get(), buf.get() + size - 1); //这里减一是吧最后的'\0'丢掉，std::string不需要'\0'作为结尾
}

}

enum class log_level {
    DEBUG=0,
    INFO,
    WARN,
    ERROR
};


class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    //禁用复制构造和赋值构造 
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template<typename... Args>
    void log(log_level level,details::with_source_location<std::string> with_loc, Args... args) {
        std::lock_guard<std::mutex> lock(mtx);  //保证线程安全
        auto const &loc = with_loc.location();  //获取调用函数的信息（函数名、行号等）
        auto msg = details::string_format(with_loc.format(), args...);
        output_log(level,msg,loc);
    }

private:
    std::mutex mtx;
    log_level g_max_level;
    //默认构造函数为private
    Logger():g_max_level(log_level::DEBUG){}


    void output_log(log_level level, std::string &msg, std::source_location const &loc) {
        const char* level_string = ""; // 用于将log_level转换为可打印的字符串
        switch (level) {
            case log_level::DEBUG: level_string = "DEBUG"; break;
            case log_level::INFO:  level_string = "INFO"; break;
            case log_level::WARN:  level_string = "WARN"; break;
            case log_level::ERROR: level_string = "ERROR"; break;
        }
        fprintf(stderr,"[%s]%s:%d->",level_string,loc.function_name(),loc.line());
        if (level >= g_max_level) {
            std::cerr<<msg<<std::endl;
        }
}
};