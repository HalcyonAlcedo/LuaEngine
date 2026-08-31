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
#include <unordered_map>
#include <mutex>
#include <windows.h>

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
    std::string scriptName;                 // 脚本名（可选）
    std::string functionName;               // 函数名（可选）
    MsgLevel level;                          // 等级
    std::string message;                     // 消息
    std::vector<CustomDataEntry> customData; // 自定义数据列表
};

class CircularBufferLogger {
public:
    CircularBufferLogger(size_t bufferSize);

    // 记录操作
    void logOperation(const std::string& scriptName, const std::string& functionName, MsgLevel level, const std::string& message, const std::vector<CustomDataEntry>& customData = {});

    // 崩溃时将缓冲区中的记录写入二进制文件
    void saveLogToFile() const;

private:
    size_t bufferSize_;
    std::unordered_map<std::string, std::vector<LogRecord>> buffers_; // 每个脚本的独立缓冲区
    // 多线程保护:主线程、addTask 工作线程、钩子线程都可能写入日志。
    // saveLogToFile 在异常过滤器中用 try_lock 获取,拿不到锁时放弃保存,
    // 避免在崩溃处理路径上死锁。
    mutable std::mutex mutex_;

    // 写入字符串到文件
    static void writeString(std::ofstream& outFile, const std::string& str);

    // 写入日志等级到文件
    static void writeLogLevel(std::ofstream& outFile, MsgLevel level);

    // 写入自定义数据列表到文件
    static void writeCustomData(std::ofstream& outFile, const std::vector<CustomDataEntry>& customData);
};

#endif // CIRCULAR_BUFFER_LOGGER_H
