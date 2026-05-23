#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>
#include "PowerMeterSCPI.h"
#include "DataLogger.h"

int main()
{
    std::cout << "=== CETC41 AV2438B Power Meter Control System ===\n" << std::endl;

    // Create instances
    PowerMeterSCPI powerMeter;
    DataLogger logger;

    // Configuration
    int gpibAddress = 0;  // Default GPIB address
    double frequency = 1000.0;  // 1000 MHz
    int attenuation_ch1 = 10;  // 10 dB
    int attenuation_ch2 = 0;   // 0 dB
    int samplingInterval = 1;  // 1 second

    // Initialize power meter
    std::cout << "Initializing power meter at GPIB address " << gpibAddress << "..." << std::endl;
    if (!powerMeter.Initialize(gpibAddress))
    {
        std::cerr << "Failed to initialize power meter: " << powerMeter.GetLastError() << std::endl;
        return -1;
    }
    std::cout << "Power meter initialized successfully." << std::endl;
    std::cout << "Device ID: " << powerMeter.GetDeviceID() << std::endl;

    // Configure power meter
    std::cout << "\nConfiguring power meter..." << std::endl;
    if (!powerMeter.SetFrequency(frequency))
    {
        std::cerr << "Failed to set frequency: " << powerMeter.GetLastError() << std::endl;
    }
    std::cout << "Frequency set to " << frequency << " MHz" << std::endl;

    if (!powerMeter.SetChannelAttenuation(1, attenuation_ch1))
    {
        std::cerr << "Failed to set CH1 attenuation: " << powerMeter.GetLastError() << std::endl;
    }
    std::cout << "Channel 1 attenuation set to " << attenuation_ch1 << " dB" << std::endl;

    if (!powerMeter.SetChannelAttenuation(2, attenuation_ch2))
    {
        std::cerr << "Failed to set CH2 attenuation: " << powerMeter.GetLastError() << std::endl;
    }
    std::cout << "Channel 2 attenuation set to " << attenuation_ch2 << " dB" << std::endl;

    // Start logging
    std::cout << "\nStarting data logging..." << std::endl;
    if (!logger.StartLogging("."))
    {
        std::cerr << "Failed to start logging" << std::endl;
        return -1;
    }
    std::cout << "Logging to file: " << logger.GetCurrentFilePath() << std::endl;

    // Data acquisition loop
    std::cout << "\nStarting data acquisition (Ctrl+C to stop)..." << std::endl;
    std::cout << "Time(s) | Channel A (dBm) | Channel B (dBm)" << std::endl;
    std::cout << "--------|-----------------|----------------" << std::endl;

    int sampleCount = 0;
    while (true)
    {
        PowerReading reading;
        if (powerMeter.ReadSingleShot(reading))
        {
            logger.LogReading(reading, frequency, gpibAddress, attenuation_ch1, attenuation_ch2);
            
            std::cout << std::setw(7) << sampleCount << " | "
                      << std::setw(15) << std::fixed << std::setprecision(2) << reading.channelA_dBm << " | "
                      << std::setw(16) << reading.channelB_dBm << std::endl;

            sampleCount++;
        }
        else
        {
            std::cerr << "Error reading data: " << powerMeter.GetLastError() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(samplingInterval));
    }

    // Cleanup
    logger.StopLogging();
    powerMeter.Shutdown();

    std::cout << "\nTotal samples logged: " << logger.GetEntriesLogged() << std::endl;
    std::cout << "Data logging completed." << std::endl;

    return 0;
}
