#include "CircularBufferLogger.h"

// 构造函数
CircularBufferLogger::CircularBufferLogger(size_t bufferSize)
    : bufferSize_(bufferSize) {
    // 创建 log 目录
    std::filesystem::create_directory("logs");
}

void CircularBufferLogger::logOperation(const std::string& scriptName, const std::string& functionName, MsgLevel level, const std::string& message, const std::vector<CustomDataEntry>& customData) {
    LogRecord record;
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    record.timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    record.scriptName = scriptName;
    record.functionName = functionName;
    record.level = level;
    record.message = message;
    record.customData = customData;

    // 将记录添加到对应脚本的缓冲区
    buffers_[scriptName].push_back(record);

    // 如果缓冲区超过大小，移除最旧的记录
    if (buffers_[scriptName].size() > bufferSize_) {
        buffers_[scriptName].erase(buffers_[scriptName].begin());
    }
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

    for (const auto& [scriptName, records] : buffers_) { // 遍历每个脚本的缓冲区
        for (const auto& record : records) {
            if (record.message.empty()) continue;

        // 将每条记录写入二进制文件
        outFile.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));
        writeString(outFile, record.scriptName);
        writeString(outFile, record.functionName);
        writeLogLevel(outFile, record.level);
        writeString(outFile, record.message);
        writeCustomData(outFile, record.customData);
    }
    }
    outFile.close();
    // 显示提示框，通知用户日志已保存
    std::string message = "怪物猎人世界主程序崩溃，崩溃记录已保存至：\n" + filename.str();
    MessageBoxA(NULL, message.c_str(), "LuaEngine崩溃提示", MB_OK | MB_ICONERROR);
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
