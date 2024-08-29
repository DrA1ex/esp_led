#pragma once

#include <cstdint>
#include <map>
#include <type_traits>

class String;

#include "../misc/event_topic.h"

template<typename PropE, typename PacketE,
        typename = std::enable_if_t<std::is_enum_v<PropE> && std::is_enum_v<PacketE>>>
struct PropertyMetadata {
    using PropEnumT = PropE;
    using PacketEnumT = PacketE;

    PropEnumT property;
    PacketEnumT packet_type;

    uint8_t value_offset;
    uint8_t value_size;

    const char *mqtt_in_topic = nullptr;
    const char *mqtt_out_topic = nullptr;
};

template<typename CfgT, typename MetaT,
        typename = std::enable_if_t<std::is_standard_layout_v<CfgT>>>
class ApplicationAbstract {
public:
    using ConfigT = CfgT;
    using MetaPropT = MetaT;
    using PropEnumT = typename MetaPropT::PropEnumT;
    using PacketEnumT = typename MetaPropT::PacketEnumT;

    virtual inline ConfigT &config() = 0;
    virtual inline const std::map<PacketEnumT, MetaPropT> &packet_meta() = 0;
    virtual inline const std::map<PropEnumT, std::vector<MetaPropT>> &property_meta() = 0;
    virtual inline const std::map<String, MetaPropT> &topic_property_meta() = 0;

    virtual inline EventTopic<PropEnumT> &event_property_changed() = 0;

    virtual void notify_property_changed(void *sender, PropEnumT param, void *arg = nullptr) = 0;
};