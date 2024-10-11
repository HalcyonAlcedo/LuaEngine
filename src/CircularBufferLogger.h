#ifndef CIRCULAR_BUFFER_LOGGER_H
#define CIRCULAR_BUFFER_LOGGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <csignal>
#include <sstream>
#include <iomanip>
#include <filesystem>

// 日志等级
enum class MsgLevel {
    INFO,
    WARNING,
    LogError,
    READ,
    WRITE
};

// 自定义数据的条目结构体
struct CustomDataEntry {
    std::string title;
    std::string content;
};

// 日志记录结构体
struct LogRecord {
    long long timestamp; // 以毫秒为单位的时间戳
    std::string functionName;                 // 函数名（可选）
    MsgLevel level;                           // 等级
    std::string message;                      // 消息
    std::vector<CustomDataEntry> customData;  // 自定义数据列表
};

class CircularBufferLogger {
public:
    CircularBufferLogger(size_t bufferSize);

    // 记录操作
    void logOperation(const std::string& functionName, MsgLevel level, const std::string& message, const std::vector<CustomDataEntry>& customData = {});

    // 崩溃时将缓冲区中的记录写入二进制文件
    void saveLogToFile() const;

private:
    size_t bufferSize_;
    std::vector<LogRecord> buffer_;
    size_t currentIndex_;

    // 捕获崩溃信号处理函数
    static void signalHandler(int signal);

    // 写入字符串到文件
    static void writeString(std::ofstream& outFile, const std::string& str);

    // 写入日志等级到文件
    static void writeLogLevel(std::ofstream& outFile, MsgLevel level);

    // 写入自定义数据列表到文件
    static void writeCustomData(std::ofstream& outFile, const std::vector<CustomDataEntry>& customData);
};

#endif // CIRCULAR_BUFFER_LOGGER_H