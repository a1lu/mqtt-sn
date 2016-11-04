//
// Copyright 2016 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "Sub.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <cstdint>

namespace mqttsn
{

namespace client
{

namespace app
{

namespace sub
{

namespace udp
{

namespace
{

typedef std::vector<std::uint8_t> DataBuf;

}  // namespace

Sub::Sub()
  : m_client(mqttsn_client_new())
{
    mqttsn_client_set_next_tick_program_callback(
        m_client.get(), &Sub::nextTickProgramCb, this);

    mqttsn_client_set_cancel_next_tick_wait_callback(
        m_client.get(), &Sub::caneclTickCb, this);

    mqttsn_client_set_send_output_data_callback(
        m_client.get(), &Sub::sendDataCb, this);

    mqttsn_client_set_gw_status_report_callback(
        m_client.get(), &Sub::gwStatusReportCb, this);

    mqttsn_client_set_connection_status_report_callback(
        m_client.get(), &Sub::connectionStatusReportCb, this);

    mqttsn_client_set_message_report_callback(
        m_client.get(), &Sub::messageReportCb, this);


    connect(
        &m_timer, SIGNAL(timeout()),
        this, SLOT(tick()));

    connect(
        &m_socket, SIGNAL(readyRead()),
        this, SLOT(readFromSocket()));

    connect(
        &m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(socketErrorOccurred(QAbstractSocket::SocketError)));

}

bool Sub::start()
{
    bool result =
        bindLocalPort() &&
        openSocket() &&
        connectToGw() &&
        mqttsn_client_start(m_client.get()) == MqttsnErrorCode_Success;


    if (result && (m_socket.state() == QUdpSocket::ConnectedState)) {
        doConnect();
    }
    return result;
}

void Sub::tick()
{
    auto ms = m_reqTimeout;
    m_reqTimeout = 0;
    mqttsn_client_tick(m_client.get(), ms);
}

void Sub::readFromSocket()
{
    DataBuf data;

    while (m_socket.hasPendingDatagrams()) {
        data.resize(m_socket.pendingDatagramSize());
        auto readBytes = m_socket.readDatagram(
            reinterpret_cast<char*>(&data[0]),
            data.size(),
            &m_lastSenderAddress,
            &m_lastSenderPort);
        assert(readBytes == static_cast<decltype(readBytes)>(data.size()));
        mqttsn_client_process_data(m_client.get(), &data[0], data.size());
    }
}

void Sub::socketErrorOccurred(QAbstractSocket::SocketError err)
{
    static_cast<void>(err);
    std::cerr << "ERROR: UDP Socket: " << m_socket.errorString().toStdString() << std::endl;
}


void Sub::nextTickProgram(unsigned ms)
{
    m_reqTimeout = ms;
    m_timer.setSingleShot(true);
    m_timer.start(ms);
}

void Sub::nextTickProgramCb(void* obj, unsigned ms)
{
    assert(obj != nullptr);
    reinterpret_cast<Sub*>(obj)->nextTickProgram(ms);
}

unsigned Sub::cancelTick()
{
    auto rem = m_timer.remainingTime();
    m_timer.stop();

    if (m_reqTimeout < static_cast<unsigned>(rem)) {
        rem = static_cast<decltype(rem)>(m_reqTimeout);
    }

    return m_reqTimeout - static_cast<unsigned>(rem);
}

unsigned Sub::caneclTickCb(void* obj)
{
    assert(obj != nullptr);
    return reinterpret_cast<Sub*>(obj)->cancelTick();
}

void Sub::sendData(const unsigned char* buf, unsigned bufLen, bool broadcast)
{
    if (broadcast) {
        broadcastData(buf, bufLen);
        return;
    }

    if (m_socket.state() != QUdpSocket::ConnectedState) {
        return;
    }

    sendDataConnected(buf, bufLen);
}

void Sub::sendDataCb(void* obj, const unsigned char* buf, unsigned bufLen, bool broadcast)
{
    assert(obj != nullptr);
    reinterpret_cast<Sub*>(obj)->sendData(buf, bufLen, broadcast);
}

void Sub::gwStatusReport(unsigned short gwId, MqttsnGwStatus status)
{
    if (status != MqttsnGwStatus_Available) {
        return;
    }

    if ((0 <= m_gwId) && (gwId != m_gwId)) {
        return;
    }

    if (m_socket.state() != QUdpSocket::ConnectedState) {
        m_socket.connectToHost(m_lastSenderAddress, m_lastSenderPort);
        if (!m_socket.waitForConnected(2000)) {
            std::cerr << "ERROR: Failed to connect UDP socket" << std::endl;
            return;
        }

        assert(m_socket.isOpen());
        assert(m_socket.state() == QUdpSocket::ConnectedState);
    }

    doConnect();
}

void Sub::gwStatusReportCb(void* obj, unsigned short gwId, MqttsnGwStatus status)
{
    assert(obj != nullptr);
    reinterpret_cast<Sub*>(obj)->gwStatusReport(gwId, status);
}

void Sub::connectionStatusReport(MqttsnConnectionStatus status)
{
    if (status == MqttsnConnectionStatus_Connected) {
        doSubscribe();
        return;
    }

    if (status == MqttsnConnectionStatus_Disconnected) {
        std::cerr << "WARNING: Disconnected from GW, reconnecting..." << std::endl;
        doConnect(true);
        return;
    }

    if (status == MqttsnConnectionStatus_Conjestion) {
        std::cerr << "WARNING: Conjestion reported, reconnecting..." << std::endl;
        doConnect();
        return;
    }
}

void Sub::connectionStatusReportCb(void* obj, MqttsnConnectionStatus status)
{
    assert(obj != nullptr);
    reinterpret_cast<Sub*>(obj)->connectionStatusReport(status);
}

void Sub::messageReport(const MqttsnMessageInfo* msgInfo)
{
    assert(msgInfo != nullptr);
    static_cast<void>(msgInfo);
    assert(!"NYI");
    // TODO
}

void Sub::messageReportCb(void* obj, const MqttsnMessageInfo* msgInfo)
{
    assert(obj != nullptr);
    reinterpret_cast<Sub*>(obj)->messageReport(msgInfo);
}

void Sub::doConnect(bool reconnecting)
{
    bool cleanSession = m_cleanSession;
    if (reconnecting) {
        cleanSession = false;
    }

    auto result = mqttsn_client_connect(m_client.get(), m_clientId.c_str(), m_keepAlive, cleanSession, nullptr);
    if (result != MqttsnErrorCode_Success) {
        std::cerr << "ERROR: Failed to connect to the gateway" << std::endl;
    }
}

void Sub::doSubscribe()
{
    // TODO:
    assert(!"Subscribing");
}

bool Sub::bindLocalPort()
{
    if (m_localPort == 0) {
        return true;
    }

    bool result =
        m_socket.bind(QHostAddress::AnyIPv4, m_localPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (!result) {
        std::cerr << "ERROR: Failed to bind UDP socket to local port " << m_localPort << std::endl;
    }
    return result;
}

bool Sub::openSocket()
{
    if (m_socket.isOpen()) {
        return true;
    }

    bool result = m_socket.open(QUdpSocket::ReadWrite);
    if (!result) {
        std::cerr << "ERROR: Failed to open UDP socket" << std::endl;
    }
    return result;
}

bool Sub::connectToGw()
{
    if ((m_gwAddr.isEmpty()) || (m_gwPort == 0)) {
        return true;
    }

    m_socket.connectToHost(m_gwAddr, m_gwPort);
    if (!m_socket.waitForConnected(2000)) {
        std::cerr << "ERROR: Failed to connect UDP socket" << std::endl;
        return false;
    }

    assert(m_socket.isOpen());
    assert(m_socket.state() == QUdpSocket::ConnectedState);

    mqttsn_client_set_searchgw_mode(m_client.get(), MqttsnSearchgwMode_Disabled);
    return true;
}

void Sub::broadcastData(const unsigned char* buf, unsigned bufLen)
{
    if (m_gwPort == 0) {
        return;
    }

    std::size_t writtenCount = 0;
    while (writtenCount < bufLen) {
        auto remSize = bufLen - writtenCount;
        auto count =
            m_socket.writeDatagram(
                reinterpret_cast<const char*>(&buf[writtenCount]),
                remSize,
                QHostAddress::Broadcast,
                m_gwPort);

        if (count < 0) {
            std::cerr << "ERROR: Failed to broadcast data" << std::endl;
            return;
        }

        writtenCount += count;
    }

}

void Sub::sendDataConnected(const unsigned char* buf, unsigned bufLen)
{
    std::size_t writtenCount = 0;
    while (writtenCount < bufLen) {
        auto remSize = bufLen - writtenCount;
        auto count =
            m_socket.write(
                reinterpret_cast<const char*>(&buf[writtenCount]),
                remSize);
        if (count < 0) {
            std::cerr << "ERROR: Failed to write to UDP socket" << std::endl;
            return;
        }

        writtenCount += count;
    }
}


}  // namespace udp

}  // namespace sub

}  // namespace app

}  // namespace client

}  // namespace mqttsn


