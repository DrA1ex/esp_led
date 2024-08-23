#pragma once

#include "Arduino.h"

enum class WifiManagerState {
    DISCONNECTED,
    CONNECTING,
    CONNECTED
};

class WifiManager {
    WifiManagerState _state = WifiManagerState::DISCONNECTED;
    byte _mode = 0;

    unsigned long _connection_interval = 0;
    unsigned long _connection_begin_time = 0;
    unsigned long _last_connection_check = 0;

public:
    void connect(byte mode, unsigned long connection_interval = 0);
    void handle_connection();

    inline WifiManagerState state() { return _state; }

private:
    void _connect_ap();
    void _connect_sta_step();
};