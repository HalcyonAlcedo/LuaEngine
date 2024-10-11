#include "CircularBufferLogger.h"

CircularBufferLogger::CircularBufferLogger(size_t bufferSize)
    : bufferSize_(bufferSize), buffer_(bufferSize), currentIndex_(0) {
    // 创建log目录
    std::filesystem::create_directory("logs");

    // 设置崩溃时的信号处理函数
    signal(SIGSEGV, signalHandler);
}

void CircularBufferLogger::logOperation(const std::string& functionName, MsgLevel level, const std::string& message, const std::vector<CustomDataEntry>& customData) {
    LogRecord record;
    // 获取当前时间（包括毫秒）
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    record.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(); // 以毫秒为单位记录时间戳
    record.functionName = functionName;
    record.level = level;
    record.message = message;
    record.customData = customData;

    // 写入环形缓冲区
    buffer_[currentIndex_] = record;
    currentIndex_ = (currentIndex_ + 1) % bufferSize_;  // 环形索引
}

void CircularBufferLogger::saveLogToFile() const {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm localTime;
    localtime_s(&localTime, &now);  // 使用 localtime_s

    std::stringstream filename;
    filename << "logs/log_" << std::put_time(&localTime, "%Y%m%d_%H%M%S") << ".crash";
    std::ofstream outFile(filename.str(), std::ios::binary);

    if (!outFile.is_open()) {
        std::cerr << "无法创建日志文件" << std::endl;
        return;
    }

    for (const auto& record : buffer_) {
        if (record.message.empty()) continue;

        // 将每条记录写入二进制文件
        outFile.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));
        writeString(outFile, record.functionName);
        writeLogLevel(outFile, record.level);
        writeString(outFile, record.message);
        writeCustomData(outFile, record.customData);  // 写入自定义数据
    }
    outFile.close();
}

void CircularBufferLogger::signalHandler(int signal) {
    // 创建日志对象并保存日志
    CircularBufferLogger logger(100);
    logger.saveLogToFile();
    exit(signal);
}

void CircularBufferLogger::writeString(std::ofstream& outFile, const std::string& str) {
    size_t length = str.size();
    outFile.write(reinterpret_cast<const char*>(&length), sizeof(length));
    outFile.write(str.c_str(), length);
}

void CircularBufferLogger::writeLogLevel(std::ofstream& outFile, MsgLevel level) {
    outFile.write(reinterpret_cast<const char*>(&level), sizeof(level));
}

void CircularBufferLogger::writeCustomData(std::ofstream& outFile, const std::vector<CustomDataEntry>& customData) {
    size_t dataSize = customData.size();
    outFile.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));

    for (const auto& entry : customData) {
        writeString(outFile, entry.title);
        writeString(outFile, entry.content);
    }
}
