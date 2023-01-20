## Sylar

## 开发环境
Ubuntu20.04
gcc 9.4

## 项目路径
```
bin --二进制文件
build --中间文件路径
cmake --cmake函数文件夹
CMakeList.txt --cmake的定义文件
lib --库的输出路径
Makefile --makefile文件
sylar --源代码文件
tests --测试代码文件

## 日志系统
1) Log4J
```
  Logger（定义日志类别）
     |
              Formatter（日志格式）
     |
  Appender（日志输出地方）
```

## 采用协程库封装

## Socket系列函数开发

## http协议

## 分布式协议

## 采用sylar框架实现一个推荐系统

## 项目亮点
1) 使用智能指针
2) Logger和LogAppender之间采用观察者模式

## Day6

初步完成了日志系统的设计和调试，将日志的几个模块条理化
Logger--日志类，包含了LogAppender(日志输出地类)，LogFormatter(日志输出格式类)。其中Appender类也有LogFormatter类对象指针，其派生的不同输出地子类的日志输出格式不同
另外声明一个格式抽象类FormatItem，含有一个纯虚函数format，对于时间，错误信息等日志信息，由相应派生类具体实现format方法。
```
Logger:
  LoggerAppender::ptr
  LoggerFormatter::ptr
-------------------------
LoggerAppender:
  LoggerFormatter::ptr
-------------------------
LoggerFormatter:
  FormatItem::ptr
-------------------------
//输出到控制台的Appender
class StdoutLogAppender : public LogAppender 
//时间日志输出器
class DateTimeFormatItem :public FormatItem
```
