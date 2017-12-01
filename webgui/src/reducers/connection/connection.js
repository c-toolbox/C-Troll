import {
    ServerConnect,
    ServerConnected,
    ServerDisconnected,
    InitializeGui

} from '../../actions';

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
        initialized: false,
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

function initializeGui(state, action) {
    return {
        ...state,
        initialized: true
    }
}

export default (state = initialConnectionState, action) => {
    switch (action.type) {
        case InitializeGui: return initializeGui(state, action);
        case ServerConnect: return serverConnect(state, action);
        case ServerDisconnected: return serverDisconnected(state, action);
        case ServerConnected: return serverConnected(state, action);
        default: return state;
    }
};