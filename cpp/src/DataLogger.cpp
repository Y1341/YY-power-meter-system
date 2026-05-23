#include "DataLogger.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdio>

DataLogger::DataLogger()
    : m_isLogging(false)
{
}

DataLogger::~DataLogger()
{
    StopLogging();
}

bool DataLogger::StartLogging(const std::string& directory)
{
    if (m_isLogging)
    {
        return false;  // Already logging
    }

    m_currentFilePath = directory + "/" + GenerateFilename();
    m_file.open(m_currentFilePath, std::ios::out | std::ios::app);

    if (!m_file.is_open())
    {
        return false;
    }

    m_isLogging = true;
    m_loggingStartTime = std::chrono::system_clock::now();
    m_entries.clear();

    return true;
}

bool DataLogger::StopLogging()
{
    if (!m_isLogging)
    {
        return false;
    }

    if (m_file.is_open())
    {
        m_file.close();
    }

    m_isLogging = false;
    return true;
}

bool DataLogger::LogReading(const PowerReading& reading,
                           double frequencyMHz,
                           int gpibAddress,
                           int channelA_attenuation_dB,
                           int channelB_attenuation_dB)
{
    if (!m_isLogging || !m_file.is_open())
    {
        return false;
    }

    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_loggingStartTime);

    // Write header on first entry
    if (m_entries.empty())
    {
        WriteHeader(frequencyMHz, gpibAddress, channelA_attenuation_dB, channelB_attenuation_dB);
    }

    // Format and write data line
    m_file << std::setw(5) << elapsed.count() << " | "
            << std::fixed << std::setprecision(2)
            << std::setw(15) << reading.channelA_dBm << " | "
            << std::setw(15) << reading.channelB_dBm << std::endl;

    m_file.flush();

    // Store in memory
    LogEntry entry;
    entry.timestamp = now;
    entry.channelA_dBm = reading.channelA_dBm;
    entry.channelB_dBm = reading.channelB_dBm;
    m_entries.push_back(entry);

    return true;
}

std::string DataLogger::GenerateFilename()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    struct tm* timeInfo = localtime(&time);

    std::ostringstream filename;
    filename << std::put_time(timeInfo, "%Y%m%d%H%M%S") << ".txt";
    return filename.str();
}

std::string DataLogger::TimestampToString(const std::chrono::system_clock::time_point& tp)
{
    auto time = std::chrono::system_clock::to_time_t(tp);
    struct tm* timeInfo = localtime(&time);

    std::ostringstream timestamp;
    timestamp << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    return timestamp.str();
}

void DataLogger::WriteHeader(double frequencyMHz, int gpibAddress,
                            int channelA_attenuation_dB, int channelB_attenuation_dB)
{
    m_file << "=== Power Meter Data Log ===\n" << std::endl;
    m_file << "Timestamp: " << TimestampToString(std::chrono::system_clock::now()) << std::endl;
    m_file << "Device: CETC41 AV2438B\n" << std::endl;
    m_file << "GPIB Address: " << gpibAddress << std::endl;
    m_file << "Frequency: " << frequencyMHz << " MHz\n" << std::endl;
    m_file << "Channel A Attenuation: " << channelA_attenuation_dB << " dB" << std::endl;
    m_file << "Channel B Attenuation: " << channelB_attenuation_dB << " dB\n" << std::endl;
    m_file << "Time(s) | Channel A (dBm) | Channel B (dBm)" << std::endl;
    m_file << "--------|-----------------|----------------" << std::endl;
}
