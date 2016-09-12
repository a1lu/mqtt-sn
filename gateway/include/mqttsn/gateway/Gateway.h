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


#pragma once

#include <memory>
#include <cstdint>

#include "mqttsn/gateway/Api.h"

namespace mqttsn
{

namespace gateway
{

class GatewayImpl;
class MQTTSN_GATEWAY_API Gateway
{
public:
    Gateway();
    ~Gateway();

    typedef std::function<void (unsigned value)> NextTickProgramReqCb;
    typedef std::function<void (const std::uint8_t* buf, std::size_t bufSize)> SendDataReqCb;

    void setNextTickProgramReqCb(NextTickProgramReqCb&& func);
    void setSendDataReqCb(SendDataReqCb&& func);

    void setAdvertisePeriod(std::uint16_t value);
    void setGatewayId(std::uint8_t value);
    bool start();
    void stop();
    void tick();

private:
    std::unique_ptr<GatewayImpl> m_pImpl;
};

}  // namespace gateway

}  // namespace mqttsn


