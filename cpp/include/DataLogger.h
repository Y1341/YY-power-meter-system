#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include "PowerMeterSCPI.h"
#include <string>
#include <fstream>
#include <vector>
#include <chrono>

struct LogEntry
{
    std::chrono::system_clock::time_point timestamp;
    double channelA_dBm;
    double channelB_dBm;
};

class DataLogger
{
public:
    DataLogger();
    ~DataLogger();

    // File management
    bool StartLogging(const std::string& directory = ".");
    bool StopLogging();
    bool IsLogging() const { return m_isLogging; }
    std::string GetCurrentFilePath() const { return m_currentFilePath; }

    // Data logging
    bool LogReading(const PowerReading& reading, 
                   double frequencyMHz,
                   int gpibAddress,
                   int channelA_attenuation_dB,
                   int channelB_attenuation_dB);

    // File info
    int GetEntriesLogged() const { return m_entries.size(); }

private:
    std::ofstream m_file;
    bool m_isLogging;
    std::string m_currentFilePath;
    std::vector<LogEntry> m_entries;
    std::chrono::system_clock::time_point m_loggingStartTime;

    // Helper functions
    std::string GenerateFilename();
    std::string TimestampToString(const std::chrono::system_clock::time_point& tp);
    void WriteHeader(double frequencyMHz, int gpibAddress, 
                    int channelA_attenuation_dB, int channelB_attenuation_dB);
};

#endif // DATA_LOGGER_H
