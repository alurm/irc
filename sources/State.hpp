#ifndef STATE_HPP
#define STATE_HPP

namespace client_state {
    enum ClientState {
        HANDSHAKE,
        LOGIN,
        REGISTERED,
        DISCONNECTED
    };

    extern ClientState state;
}

#endif
