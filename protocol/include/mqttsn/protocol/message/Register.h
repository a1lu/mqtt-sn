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

#include "comms/comms.h"
#include "mqttsn/protocol/MsgTypeId.h"
#include "mqttsn/protocol/field.h"
#include "mqttsn/protocol/ParsedOptions.h"

namespace mqttsn
{

namespace protocol
{

namespace message
{

template <typename TFieldBase, typename TOptions>
using RegisterFields =
    std::tuple<
        field::TopicId<TFieldBase>,
        field::MsgId<TFieldBase>,
        field::TopicName<TFieldBase, TOptions>
    >;

template <typename TMsgBase, typename TOptions = ParsedOptions<> >
class Register : public
    comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgTypeId_REGISTER>,
        comms::option::FieldsImpl<RegisterFields<typename TMsgBase::Field, TOptions> >,
        comms::option::DispatchImpl<Register<TMsgBase, TOptions> >
    >
{
    typedef comms::MessageBase<
        TMsgBase,
        comms::option::StaticNumIdImpl<MsgTypeId_REGISTER>,
        comms::option::FieldsImpl<RegisterFields<typename TMsgBase::Field, TOptions> >,
        comms::option::DispatchImpl<Register<TMsgBase, TOptions> >
    > Base;

public:
    enum FieldIdx
    {
        FieldIdx_topicId,
        FieldIdx_msgId,
        FieldIdx_topicName,
        FieldIdx_numOfValues
    };

    static_assert(std::tuple_size<typename Base::AllFields>::value == FieldIdx_numOfValues,
        "Number of fields is incorrect");
};

}  // namespace message

}  // namespace protocol

}  // namespace mqttsn

