#include "PowerMeterSCPI.h"
#include <sstream>
#include <iomanip>

PowerMeterSCPI::PowerMeterSCPI()
    : m_visa(std::make_unique<VisaDevice>()),
      m_initialized(false),
      m_currentChannel(1),
      m_frequencyMHz(1000.0),
      m_attenuation_ch1(0.0),
      m_attenuation_ch2(0.0)
{
}

PowerMeterSCPI::~PowerMeterSCPI()
{
    Shutdown();
}

bool PowerMeterSCPI::Initialize(int gpibAddress)
{
    if (!m_visa->Connect(gpibAddress))
    {
        SetError(m_visa->GetLastError());
        return false;
    }

    // Query device ID to verify connection
    std::string deviceID = GetDeviceID();
    if (deviceID.empty())
    {
        SetError("Failed to query device ID");
        m_visa->Disconnect();
        return false;
    }

    // Reset device to known state
    if (!Reset())
    {
        SetError("Failed to reset device");
        m_visa->Disconnect();
        return false;
    }

    m_initialized = true;
    return true;
}

bool PowerMeterSCPI::Shutdown()
{
    if (m_visa)
    {
        m_visa->Disconnect();
    }
    m_initialized = false;
    return true;
}

bool PowerMeterSCPI::Reset()
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return false;
    }

    return m_visa->Reset();
}

bool PowerMeterSCPI::SetFrequency(double frequencyMHz)
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return false;
    }

    // Convert MHz to Hz for SCPI command
    double frequencyHz = frequencyMHz * 1e6;
    std::ostringstream command;
    command << std::fixed << std::setprecision(0) << ":CONF:FREQ " << frequencyHz;

    if (!m_visa->SendCommand(command.str()))
    {
        SetError(m_visa->GetLastError());
        return false;
    }

    m_frequencyMHz = frequencyMHz;
    return true;
}

double PowerMeterSCPI::GetFrequency()
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return -1.0;
    }

    std::string response = m_visa->QueryCommand(":CONF:FREQ?");
    double frequency = -1.0;
    if (ParseDoubleResponse(response, frequency))
    {
        m_frequencyMHz = frequency / 1e6;  // Convert Hz to MHz
        return m_frequencyMHz;
    }

    SetError("Failed to parse frequency response");
    return -1.0;
}

bool PowerMeterSCPI::SetChannelAttenuation(int channel, double attenuationdB)
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return false;
    }

    if (channel < 1 || channel > 2)
    {
        SetError("Invalid channel: " + std::to_string(channel));
        return false;
    }

    // Select channel first
    if (!SelectChannel(channel))
    {
        return false;
    }

    // Set attenuation
    std::ostringstream command;
    command << std::fixed << std::setprecision(1) << ":INP:ATT " << attenuationdB;

    if (!m_visa->SendCommand(command.str()))
    {
        SetError(m_visa->GetLastError());
        return false;
    }

    if (channel == 1)
        m_attenuation_ch1 = attenuationdB;
    else
        m_attenuation_ch2 = attenuationdB;

    return true;
}

double PowerMeterSCPI::GetChannelAttenuation(int channel)
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return -1.0;
    }

    if (channel < 1 || channel > 2)
    {
        SetError("Invalid channel: " + std::to_string(channel));
        return -1.0;
    }

    if (!SelectChannel(channel))
    {
        return -1.0;
    }

    std::string response = m_visa->QueryCommand(":INP:ATT?");
    double attenuation = -1.0;
    if (ParseDoubleResponse(response, attenuation))
    {
        return attenuation;
    }

    SetError("Failed to parse attenuation response");
    return -1.0;
}

bool PowerMeterSCPI::ReadSingleShot(PowerReading& reading)
{
    if (!m_initialized)
    {
        SetError("Device not initialized");
        return false;
    }

    // Read Channel 1
    if (!SelectChannel(1))
    {
        return false;
    }

    std::string response1 = m_visa->QueryCommand(":READ:POW?");
    if (!ParseDoubleResponse(response1, reading.channelA_dBm))
    {
        SetError("Failed to parse Channel 1 power reading");
        return false;
    }
    reading.channelA_attenuation = m_attenuation_ch1;

    // Read Channel 2
    if (!SelectChannel(2))
    {
        return false;
    }

    std::string response2 = m_visa->QueryCommand(":READ:POW?");
    if (!ParseDoubleResponse(response2, reading.channelB_dBm))
    {
        SetError("Failed to parse Channel 2 power reading");
        return false;
    }
    reading.channelB_attenuation = m_attenuation_ch2;

    return true;
}

bool PowerMeterSCPI::SelectChannel(int channel)
{
    if (channel < 1 || channel > 2)
    {
        SetError("Invalid channel: " + std::to_string(channel));
        return false;
    }

    if (m_currentChannel == channel)
    {
        return true;  // Already on this channel
    }

    std::ostringstream command;
    command << ":CONF:CHANNEL " << channel;

    if (!m_visa->SendCommand(command.str()))
    {
        SetError(m_visa->GetLastError());
        return false;
    }

    m_currentChannel = channel;
    return true;
}

std::string PowerMeterSCPI::GetDeviceID()
{
    if (!m_visa->IsConnected())
    {
        SetError("Device not connected");
        return "";
    }

    return m_visa->QueryCommand("*IDN?");
}

bool PowerMeterSCPI::ParseDoubleResponse(const std::string& response, double& value)
{
    try
    {
        value = std::stod(response);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void PowerMeterSCPI::SetError(const std::string& error)
{
    m_lastError = error;
}
