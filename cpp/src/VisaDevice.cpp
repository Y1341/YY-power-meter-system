#include "VisaDevice.h"
#include <sstream>
#include <cstring>

VisaDevice::VisaDevice()
    : m_defaultRM(VI_NULL), m_instrument(VI_NULL), m_isConnected(false), m_lastStatus(VI_SUCCESS)
{
}

VisaDevice::~VisaDevice()
{
    Disconnect();
}

bool VisaDevice::Connect(int gpibAddress, int timeout)
{
    // Open default VISA resource manager
    m_lastStatus = viOpenDefaultRM(&m_defaultRM);
    if (m_lastStatus < VI_SUCCESS)
    {
        SetError("Failed to open VISA resource manager", m_lastStatus);
        return false;
    }

    // Build GPIB resource string
    std::ostringstream resourceString;
    resourceString << "GPIB0::" << gpibAddress << "::INSTR";
    std::string resource = resourceString.str();

    // Open instrument session
    m_lastStatus = viOpen(m_defaultRM, (ViRsrc)resource.c_str(), VI_NULL, timeout, &m_instrument);
    if (m_lastStatus < VI_SUCCESS)
    {
        SetError("Failed to open GPIB instrument at address " + std::to_string(gpibAddress), m_lastStatus);
        viClose(m_defaultRM);
        m_defaultRM = VI_NULL;
        return false;
    }

    // Set communication parameters
    viSetAttribute(m_instrument, VI_ATTR_ASRL_BAUD_RATE, 19200);
    viSetAttribute(m_instrument, VI_ATTR_ASRL_DATA_BITS, 8);
    viSetAttribute(m_instrument, VI_ATTR_ASRL_STOP_BITS, VI_ASRL_STOP_ONE);
    viSetAttribute(m_instrument, VI_ATTR_ASRL_PARITY, VI_ASRL_PAR_NONE);
    viSetAttribute(m_instrument, VI_ATTR_TMO_VALUE, timeout);

    m_isConnected = true;
    m_lastError = "Connection successful";
    return true;
}

bool VisaDevice::Disconnect()
{
    if (m_instrument != VI_NULL)
    {
        viClose(m_instrument);
        m_instrument = VI_NULL;
    }

    if (m_defaultRM != VI_NULL)
    {
        viClose(m_defaultRM);
        m_defaultRM = VI_NULL;
    }

    m_isConnected = false;
    return true;
}

bool VisaDevice::SendCommand(const std::string& command)
{
    if (!m_isConnected)
    {
        SetError("Device not connected", VI_ERROR_TMO);
        return false;
    }

    std::string cmdWithNewline = command + "\n";
    ViUInt32 bytesWritten = 0;
    m_lastStatus = viWrite(m_instrument, (ViBuf)(char*)cmdWithNewline.c_str(),
                          (ViUInt32)cmdWithNewline.length(), &bytesWritten);

    if (m_lastStatus < VI_SUCCESS)
    {
        SetError("Failed to send command: " + command, m_lastStatus);
        return false;
    }

    return true;
}

std::string VisaDevice::QueryCommand(const std::string& command)
{
    if (!m_isConnected)
    {
        SetError("Device not connected", VI_ERROR_TMO);
        return "";
    }

    // Send query command
    if (!SendCommand(command))
    {
        return "";
    }

    // Read response
    ViBuf response = new ViByte[256];
    ViUInt32 bytesRead = 0;
    m_lastStatus = viRead(m_instrument, response, 255, &bytesRead);

    std::string result;
    if (m_lastStatus >= VI_SUCCESS && bytesRead > 0)
    {
        response[bytesRead] = '\0';
        result = std::string((char*)response);
        // Remove trailing newline/whitespace
        result.erase(result.find_last_not_of(" \n\r\t") + 1);
    }
    else
    {
        SetError("Failed to read response from device", m_lastStatus);
    }

    delete[] response;
    return result;
}

bool VisaDevice::Reset()
{
    return SendCommand("*RST");
}

void VisaDevice::SetError(const std::string& error, ViStatus status)
{
    m_lastError = error + " (Status: " + std::to_string(status) + ")";
    m_lastStatus = status;
}

std::string VisaDevice::VisaStatusToString(ViStatus status)
{
    switch (status)
    {
        case VI_SUCCESS: return "Success";
        case VI_ERROR_TMO: return "Timeout";
        case VI_ERROR_CONN_LOST: return "Connection lost";
        case VI_ERROR_INV_RSRC: return "Invalid resource";
        default: return "Unknown error (" + std::to_string(status) + ")";
    }
}
