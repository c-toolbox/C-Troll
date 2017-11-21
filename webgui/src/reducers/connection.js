import {
    ServerConnect,
    ServerConnected

} from '../actions';

const initialConnectionState = {
    connecting: false,
    connectionLost: false,
    initialized: false,
    connected: false
}


function serverConnect(state, action) {
    return {
        ...state,
        connected: false,
        connecting: true
    }
}

function serverDisconnected(state, action) {
    return {
        ...state,
        connected: false,
        connectionLost: true
    }
}

function serverConnected(state, action) {
    return {
        ...state,
        connected: true,
        connecting: false,
        connectionLost: false
    }
}

export default (state = initialConnectionState, action) => {
    switch (action.type) {
        case ServerConnect: return serverConnect(state, action);
        case serverDisconnected: return serverDisconnected(state, action);
        case ServerConnected: return serverConnected(state, action);
        default: return state;
    }
};