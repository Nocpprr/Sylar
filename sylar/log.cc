#include "log.h"
#include <map>
#include <iostream>
#include <functional>
#include <ctime>
#include <iomanip>
namespace sylar{

const char* LogLevel::ToString(LogLevel::Level level) {
    switch(level) {
        // 使用宏定义一个XX(name)来优化switch, XX(name) 被展开为case语句
#define XX(name) \
        case LogLevel::name: \
            return #name; \
            break;
        
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
#undef XX
        default: 
            return "UNKNOW";
    }
    return "UNKNOW";
}


class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is MessageFormatItem" << std::endl;};
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getContent();
        }
};

class LevelFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is LevelFormatItem" << std::endl;};
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << LogLevel::ToString(level);
        }
};

class ElapseFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is ElapseFormatItem" << std::endl;}
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getElapse();
        }
};

class NameFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is NameFormatItem" << std::endl;}
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
};

class ThreadIdFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is ThreadIdFormatItem" << std::endl;}
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
};

class FiberIdFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is FiberIdFormatItem" << std::endl;}
        FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
};

class DateTimeFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is DateTimeFormatItem" << std::endl;}
        DateTimeFormatItem(const std::string& format = "%Y:%m:%d %H:%M:%S") 
            : m_format(format) {
            if(m_format.empty()) {
                m_format = "%Y:%m:%d %H:%M:%S";
            }

        }
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            os << std::put_time(&tm, m_format.data());
        }
    private:
        std::string m_format;
};

class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is FilenameFormatItem" << std::endl;}
        FilenameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
};

class LineFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is LineFormatItem" << std::endl;}
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
};

class NewLineFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is NewLineFormatItem" << std::endl;}
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
};

class StringFormatItem :public LogFormatter::FormatItem {
    public:
        void Print() { std::cout << "This is StringFormatItem" << std::endl;}
        StringFormatItem(const std::string& str) 
            : m_string(str) {

        }
        void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string ;
        }
    private:
        std::string m_string;
};

LogEvent::LogEvent(const char* file, int32_t line, uint32_t elapse
    , uint32_t thread_id, uint32_t fiber_id, uint64_t time) 
    :m_file(file)
    ,m_line(line)
    ,m_elapse(elapse)
    ,m_threadId(thread_id)
    ,m_fiberId(fiber_id)
    ,m_time(time)
    // ,m_threadName(thread_name)
    // ,m_logger(logger)
    // ,m_level(level) 
    {

}

Logger::Logger(const std::string& name)
    : m_name(name)
    , m_level(LogLevel::DEBUG) {
    m_formatter.reset(new LogFormatter("%d [%p] <%f:%l> %m %n"));

}

void Logger::addAppender(LogAppender::ptr appender) {
    if(!appender->getFormatter()) {
        appender->setFormatter(m_formatter);
    }
    m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender) {
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
        if(*it == appender) {
            m_appenders.erase(it);
            break;
        }
    }
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
     if(level >= m_level) {
        auto self = shared_from_this();

        for(auto& i : m_appenders) {
            i->log(self, level, event); 
        }
     }
}

void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);

}
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);

}
void Logger::warn(LogEvent::ptr event) {
    log(LogLevel::WARN, event);

}
void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);

}
void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);

}

FileLogAppender::FileLogAppender(const std::string& filename) 
    : m_filename(filename) {

}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if(level >= m_level) {
        m_filestream << m_formatter->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if(m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;          //!!表示非0值转为1，0值不变
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr  event) {
    if(level >= m_level) {
        //std::cout << "Stdout" << std::endl;
        std::string str = m_formatter->format(logger, level, event);
        std::cout << str << std::endl;
    }
}

LogFormatter::LogFormatter(const std::string& pattern) 
    : m_pattern(pattern) {
    //std::cout << "init" << std::endl;
    init();
}   

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    //std::cout << "Format" << std::endl;
    for(auto i : m_items) {
        //i->Print();
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

// 解析pattern格式
// %d [%p] <%f:%l> %m %n
void LogFormatter::init() {
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FilenameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        // XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIdFormatItem),           //F:协程id
        // XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };
    //std::cout << m_pattern << std::endl;
    for(decltype(m_pattern.size()) i = 0; i < m_pattern.size();) {
        if(m_pattern[i] == '%') {
            ++i;
            auto it = s_format_items.find(std::string(1, m_pattern[i]));
            if(it == s_format_items.end()) {
                m_items.emplace_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::string(1, m_pattern[i]) + ">>")));
            } else {
                if(m_pattern[i] == 'd') {
                    if(m_pattern[i + 1] == '{') {
                        auto j = i + 1;
                        while(m_pattern[j] != '}' && j < m_pattern.size()) {
                            ++j;
                        }
                        std::cout << m_pattern.substr(i + 2, j - i - 2) << std::endl;
                        m_items.emplace_back(it->second(m_pattern.substr(i + 2, j - i - 2)));
                        i = j + 1;
                    }
                    else {
                        m_items.emplace_back(it->second(std::string()));
                        ++i;
                    }
                }
                else {
                    m_items.emplace_back(it->second(std::string()));
                    ++i;
                }
            }
        }
        else {
            auto j = i;
            while(m_pattern[j] != '%' && j < m_pattern.size()) {
                ++j;
            }
            //std::cout << m_pattern.substr(i, j - i) << std::endl;
            m_items.emplace_back(FormatItem::ptr(new StringFormatItem(m_pattern.substr(i, j - i))));
            i = j;
        }
    }
}

// void LogFormatter::init() {
//     //str, format, type
//     std::vector<std::tuple<std::string, std::string, int> > vec;
//     std::string nstr;
//     for(size_t i = 0; i < m_pattern.size(); ++i) {
//         if(m_pattern[i] != '%') {
//             nstr.append(1, m_pattern[i]);
//             continue;
//         }

//         if((i + 1) < m_pattern.size()) {
//             if(m_pattern[i + 1] == '%') {
//                 nstr.append(1, '%');
//                 continue;
//             }
//         }

//         size_t n = i + 1;
//         int fmt_status = 0;
//         size_t fmt_begin = 0;

//         std::string str;
//         std::string fmt;
//         while(n < m_pattern.size()) {
//             if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
//                     && m_pattern[n] != '}')) {
//                 str = m_pattern.substr(i + 1, n - i - 1);
//                 break;
//             }
//             if(fmt_status == 0) {
//                 if(m_pattern[n] == '{') {
//                     str = m_pattern.substr(i + 1, n - i - 1);
//                     //std::cout << "*" << str << std::endl;
//                     fmt_status = 1; //解析格式
//                     fmt_begin = n;
//                     ++n;
//                     continue;
//                 }
//             } else if(fmt_status == 1) {
//                 if(m_pattern[n] == '}') {
//                     fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
//                     //std::cout << "#" << fmt << std::endl;
//                     fmt_status = 0;
//                     ++n;
//                     break;
//                 }
//             }
//             ++n;
//             if(n == m_pattern.size()) {
//                 if(str.empty()) {
//                     str = m_pattern.substr(i + 1);
//                 }
//             }
//         }

//         if(fmt_status == 0) {
//             if(!nstr.empty()) {
//                 vec.push_back(std::make_tuple(nstr, std::string(), 0));
//                 nstr.clear();
//             }
//             vec.push_back(std::make_tuple(str, fmt, 1));
//             i = n - 1;
//         } else if(fmt_status == 1) {
//             std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
//             //m_error = true;
//             vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
//         }
//     }

//     if(!nstr.empty()) {
//         vec.push_back(std::make_tuple(nstr, "", 0));
//     }
//     static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
// #define XX(str, C) 
//         {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

//         XX(m, MessageFormatItem),           //m:消息
//         XX(p, LevelFormatItem),             //p:日志级别
//         XX(r, ElapseFormatItem),            //r:累计毫秒数
//         XX(c, NameFormatItem),              //c:日志名称
//         XX(t, ThreadIdFormatItem),          //t:线程id
//         XX(n, NewLineFormatItem),           //n:换行
//         XX(d, DateTimeFormatItem),          //d:时间
//         XX(f, FilenameFormatItem),          //f:文件名
//         XX(l, LineFormatItem),              //l:行号
//         //XX(T, TabFormatItem),               //T:Tab
//         XX(F, FiberIdFormatItem),           //F:协程id
//         //XX(N, ThreadNameFormatItem),        //N:线程名称
// #undef XX
//     };

//     for(auto& i : vec) {


//         if(std::get<2>(i) == 0) {
//             m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
//         } else {
//             auto it = s_format_items.find(std::get<0>(i));
//             if(it == s_format_items.end()) {
//                 m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
//                 //m_error = true;
//             } else {
//                 m_items.push_back(it->second(std::get<1>(i)));
//             }
//         }

//         std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
//     }
//     //std::cout << m_items.size() << std::endl;
// }

}

