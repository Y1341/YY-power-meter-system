#ifndef VISA_DEVICE_H
#define VISA_DEVICE_H

#include <string>
#include <visa.h>

class VisaDevice
{
public:
    VisaDevice();
    ~VisaDevice();

    // Connection management
    bool Connect(int gpibAddress, int timeout = 2000);
    bool Disconnect();
    bool IsConnected() const { return m_isConnected; }

    // SCPI communication
    bool SendCommand(const std::string& command);
    std::string QueryCommand(const std::string& command);
    bool Reset();

    // Error handling
    std::string GetLastError() const { return m_lastError; }
    ViStatus GetLastStatus() const { return m_lastStatus; }

private:
    ViSession m_defaultRM;
    ViSession m_instrument;
    bool m_isConnected;
    std::string m_lastError;
    ViStatus m_lastStatus;

    // Helper functions
    void SetError(const std::string& error, ViStatus status);
    std::string VisaStatusToString(ViStatus status);
};

#endif // VISA_DEVICE_H
