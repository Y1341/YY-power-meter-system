#ifndef POWER_METER_SCPI_H
#define POWER_METER_SCPI_H

#include "VisaDevice.h"
#include <string>
#include <vector>
#include <memory>

struct PowerReading
{
    double channelA_dBm;  // Channel A power in dBm
    double channelB_dBm;  // Channel B power in dBm
    double channelA_attenuation;  // Channel A attenuation in dB
    double channelB_attenuation;  // Channel B attenuation in dB
};

class PowerMeterSCPI
{
public:
    PowerMeterSCPI();
    ~PowerMeterSCPI();

    // Device management
    bool Initialize(int gpibAddress);
    bool Shutdown();
    bool Reset();
    bool IsInitialized() const { return m_initialized; }

    // Configuration
    bool SetFrequency(double frequencyMHz);
    double GetFrequency();
    
    bool SetChannelAttenuation(int channel, double attenuationdB);
    double GetChannelAttenuation(int channel);

    // Data reading
    bool ReadSingleShot(PowerReading& reading);
    bool SelectChannel(int channel);  // 1 or 2

    // Device info
    std::string GetDeviceID();
    std::string GetLastError() const { return m_lastError; }

private:
    std::unique_ptr<VisaDevice> m_visa;
    bool m_initialized;
    std::string m_lastError;
    int m_currentChannel;
    double m_frequencyMHz;
    double m_attenuation_ch1, m_attenuation_ch2;

    // Helper functions
    bool ParseDoubleResponse(const std::string& response, double& value);
    void SetError(const std::string& error);
};

#endif // POWER_METER_SCPI_H
