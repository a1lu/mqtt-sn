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

#include "mqttsn/protocol/Message.h"
#include "mqttsn/protocol/Stack.h"
#include "mqtt/protocol/v311/Message.h"
#include "mqtt/protocol/v311/Stack.h"

namespace mqttsn
{

namespace gateway
{

class MsgHandler;

typedef mqttsn::protocol::MessageT<
    comms::option::IdInfoInterface,
    comms::option::ReadIterator<const std::uint8_t*>,
    comms::option::WriteIterator<std::uint8_t*>,
    comms::option::Handler<MsgHandler>,
    comms::option::LengthInfoInterface
> MqttsnMessage;

typedef mqtt::protocol::v311::MessageT<
    comms::option::IdInfoInterface,
    comms::option::ReadIterator<const std::uint8_t*>,
    comms::option::WriteIterator<std::uint8_t*>,
    comms::option::Handler<MsgHandler>,
    comms::option::LengthInfoInterface
> MqttMessage;

typedef protocol::ParsedOptions<
    protocol::option::GatewayOnlyVariant,
    protocol::option::UseOrigDataView
> GwOptions;

typedef mqttsn::protocol::message::Advertise<MqttsnMessage, GwOptions> AdvertiseMsg_SN;
typedef mqttsn::protocol::message::Searchgw<MqttsnMessage, GwOptions> SearchgwMsg_SN;
typedef mqttsn::protocol::message::Gwinfo<MqttsnMessage, GwOptions> GwinfoMsg_SN;
typedef mqttsn::protocol::message::Connect<MqttsnMessage, GwOptions> ConnectMsg_SN;
typedef mqttsn::protocol::message::Connack<MqttsnMessage, GwOptions> ConnackMsg_SN;
typedef mqttsn::protocol::message::Willtopicreq<MqttsnMessage, GwOptions> WilltopicreqMsg_SN;
typedef mqttsn::protocol::message::Willtopic<MqttsnMessage, GwOptions> WilltopicMsg_SN;
typedef mqttsn::protocol::message::Willmsgreq<MqttsnMessage, GwOptions> WillmsgreqMsg_SN;
typedef mqttsn::protocol::message::Willmsg<MqttsnMessage, GwOptions> WillmsgMsg_SN;
typedef mqttsn::protocol::message::Register<MqttsnMessage, GwOptions> RegisterMsg_SN;
typedef mqttsn::protocol::message::Regack<MqttsnMessage> RegackMsg_SN;
typedef mqttsn::protocol::message::Publish<MqttsnMessage, GwOptions> PublishMsg_SN;
typedef mqttsn::protocol::message::Puback<MqttsnMessage> PubackMsg_SN;
typedef mqttsn::protocol::message::Pubrec<MqttsnMessage> PubrecMsg_SN;
typedef mqttsn::protocol::message::Pubrel<MqttsnMessage> PubrelMsg_SN;
typedef mqttsn::protocol::message::Pubcomp<MqttsnMessage> PubcompMsg_SN;
typedef mqttsn::protocol::message::Subscribe<MqttsnMessage, GwOptions> SubscribeMsg_SN;
typedef mqttsn::protocol::message::Suback<MqttsnMessage, GwOptions> SubackMsg_SN;
typedef mqttsn::protocol::message::Unsubscribe<MqttsnMessage, GwOptions> UnsubscribeMsg_SN;
typedef mqttsn::protocol::message::Unsuback<MqttsnMessage> UnsubackMsg_SN;
typedef mqttsn::protocol::message::Pingreq<MqttsnMessage, GwOptions> PingreqMsg_SN;
typedef mqttsn::protocol::message::Pingresp<MqttsnMessage> PingrespMsg_SN;
typedef mqttsn::protocol::message::Disconnect<MqttsnMessage> DisconnectMsg_SN;
typedef mqttsn::protocol::message::Willtopicupd<MqttsnMessage, GwOptions> WilltopicupdMsg_SN;
typedef mqttsn::protocol::message::Willtopicresp<MqttsnMessage, GwOptions> WilltopicrespMsg_SN;
typedef mqttsn::protocol::message::Willmsgupd<MqttsnMessage, GwOptions> WillmsgupdMsg_SN;
typedef mqttsn::protocol::message::Willmsgresp<MqttsnMessage, GwOptions> WillmsgrespMsg_SN;

template <typename TMsgBase>
using InputMqttsnMessages =
    std::tuple<
    protocol::message::Searchgw<TMsgBase, GwOptions>,
    protocol::message::Connect<TMsgBase, GwOptions>,
    protocol::message::Willtopic<TMsgBase, GwOptions>,
    protocol::message::Willmsg<TMsgBase, GwOptions>,
    protocol::message::Register<TMsgBase, GwOptions>,
    protocol::message::Regack<TMsgBase>,
    protocol::message::Publish<TMsgBase, GwOptions>,
    protocol::message::Puback<TMsgBase>,
    protocol::message::Pubcomp<TMsgBase>,
    protocol::message::Pubrec<TMsgBase>,
    protocol::message::Pubrel<TMsgBase>,
    protocol::message::Subscribe<TMsgBase, GwOptions>,
    protocol::message::Unsubscribe<TMsgBase, GwOptions>,
    protocol::message::Pingreq<TMsgBase, GwOptions>,
    protocol::message::Pingresp<TMsgBase>,
    protocol::message::Disconnect<TMsgBase>,
    protocol::message::Willtopicupd<TMsgBase, GwOptions>,
    protocol::message::Willmsgupd<TMsgBase, GwOptions>
>;

typedef mqttsn::protocol::Stack<MqttsnMessage, InputMqttsnMessages<MqttsnMessage> > MqttsnProtStack;

typedef mqtt::protocol::v311::message::Connect<MqttMessage> ConnectMsg;
typedef mqtt::protocol::v311::message::Connack<MqttMessage> ConnackMsg;
typedef mqtt::protocol::v311::message::Publish<MqttMessage> PublishMsg;
typedef mqtt::protocol::v311::message::Puback<MqttMessage> PubackMsg;
typedef mqtt::protocol::v311::message::Pubrec<MqttMessage> PubrecMsg;
typedef mqtt::protocol::v311::message::Pubrel<MqttMessage> PubrelMsg;
typedef mqtt::protocol::v311::message::Pubcomp<MqttMessage> PubcompMsg;
typedef mqtt::protocol::v311::message::Subscribe<MqttMessage> SubscribeMsg;
typedef mqtt::protocol::v311::message::Suback<MqttMessage> SubackMsg;
typedef mqtt::protocol::v311::message::Unsubscribe<MqttMessage> UnsubscribeMsg;
typedef mqtt::protocol::v311::message::Unsuback<MqttMessage> UnsubackMsg;
typedef mqtt::protocol::v311::message::Pingreq<MqttMessage> PingreqMsg;
typedef mqtt::protocol::v311::message::Pingresp<MqttMessage> PingrespMsg;
typedef mqtt::protocol::v311::message::Disconnect<MqttMessage> DisconnectMsg;
typedef mqtt::protocol::v311::Stack<MqttMessage> MqttProtStack;

}  // namespace gateway

}  // namespace mqttsn


