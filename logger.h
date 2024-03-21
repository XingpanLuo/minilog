/*
https://github.com/XingpanLuo/minilog by Luoxpan
2024.3.21
单例模式，可输出调用函数信息
使用方法
main.cpp:
#include "logger.h"
using namespace std;
Logger& logger = Logger::getInstance();
int main() {
    int n=10;
    logger.set_log_level(log_level::WARN);  //低于WARN等级的不输出
    cerr<<"#######set_log_level(log_level::WARN)############\n";
    logger.debug("This is a debug message n=%d",n); 
    logger.info("This is a info");     
    logger.warn("This is a warning:(%d,%d)",2,3);
    logger.error("This is a error,n=%d");
    return 0;
}

*/

#pragma once
#include <bits/stdc++.h>
#include <source_location>

namespace details { //放在一个子命名空间，用户使用logger并不需要关心这里面的东西
template <class T>
struct with_source_location {
private:
    T fmt;
    std::source_location loc;

public:
    //使用万能引用和forward保留fmt的左右值类型
    //loc为调用函数的信息，必须使用std::move()语义，以后用的loc都是最开始调用时根据默认参数产生这个
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
    void debug(details::with_source_location<std::string> with_loc, Args&&... args){
        generate_log(log_level::DEBUG,std::move(with_loc),std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(details::with_source_location<std::string> with_loc, Args&&... args){
        generate_log(log_level::INFO,std::move(with_loc),std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warn(details::with_source_location<std::string> with_loc, Args&&... args){
        generate_log(log_level::WARN,std::move(with_loc),std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(details::with_source_location<std::string> with_loc, Args&&... args){
        generate_log(log_level::ERROR,std::move(with_loc),std::forward<Args>(args)...);
    }

    void set_log_level(log_level lev){
        g_max_level=lev;
    }

private:
    std::mutex mtx;
    log_level g_max_level;
    //默认构造函数为private,默认等级为DEBUG
    Logger():g_max_level(log_level::DEBUG){}


    void output_log(log_level level, std::string &msg, std::source_location const &loc) {
        const char* level_string = ""; // 用于将log_level转换为可打印的字符串
        switch (level) {
            case log_level::DEBUG: level_string = "DEBUG"; break;
            case log_level::INFO:  level_string = "INFO"; break;
            case log_level::WARN:  level_string = "WARN"; break;
            case log_level::ERROR: level_string = "ERROR"; break;
        }
        if (level >= g_max_level) {
            fprintf(stderr,"[%s]%s:%d->",level_string,loc.function_name(),loc.line());
            std::cerr<<msg<<std::endl;
        }
    }

    template<typename... Args>
    void generate_log(log_level level,details::with_source_location<std::string> with_loc, Args&&... args) {
        std::lock_guard<std::mutex> lock(mtx);  //保证线程安全
        auto const &loc = with_loc.location();  //获取调用函数的信息（函数名、行号等）
        
        std::string msg="";
        if constexpr (sizeof...(args) == 0){
            msg=with_loc.format();
        }else{
            msg = details::string_format(with_loc.format(),std::forward<Args>(args)...);
        }
        output_log(level,msg,loc);
    }
};