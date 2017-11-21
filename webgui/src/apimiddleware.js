import SockJS from 'sockjs-client';
import { 
    ServerConnect,
    StartProcess,
    RestartProcess,
    StopProcess,
    initializeGui,
    serverConnected,
    serverDisconnected } from './actions';

let socket = null;

function sendCommand(type, payload) {
    const message = {
        type,
        payload
    };
    socket.send(JSON.stringify(message));
}

function sendStartProcessCommand(applicationId, configurationId, clusterId) {
    const data = {
        applicationId: applicationId,
        configurationId: configurationId,
        clusterId: clusterId
    };
    sendCommand('GuiStartCommand', data);
}

function sendStopProcessCommand(processId) {
    const data = {
        command: 'Stop',
        processId: processId
    };
    sendCommand('GuiProcessCommand', data);
}

function sendRestartProcessCommand(processId) {
    const data = {
        command: 'Restart',
        processId: processId
    };
    sendCommand('GuiProcessCommand', data);
}

function serverConnect(dispatch) {
    if (socket) {
        socket.close();
    }
    socket = new SockJS('/api');
    socket.onopen = () => {
        dispatch(serverConnected());
    };

    socket.onclose = () => {
        dispatch(serverDisconnected());
    };

    socket.onmessage = (message) => {
        const data = JSON.parse(message.data);
        switch (data.type) {
            case 'GuiInit':
                dispatch(initializeGui(data.payload));
                break;
            case 'GuiProcessStatus':
                //this.handleProcessStatus(data.payload);
                break;
            case 'GuiProcessLogMessage':
                //this.handleProcessLogMessage(data.payload);
                break;
            case 'GuiProcessLogMessageHistory':
                //this.handleProcessLogMessageHistory(data.payload);
                break;
            default:
                console.log('unknown message type: "' + data.type + '"', data.payload);
        }
    };
}

const apiMiddleware = store => next => action => {
    const payload = action.payload;
    switch (action.type) {
        case ServerConnect:
            serverConnect(store.dispatch);
        break;
        case StartProcess:
            sendStartProcessCommand(
                payload.applicationId,
                payload.configurationId,
                payload.clusterId,
            );
        break;
        case RestartProcess:
            sendRestartProcessCommand(
                payload.applicationId,
                payload.configurationId,
                payload.clusterId,
            );
        break;
        case StopProcess:
            sendStopProcessCommand(
                payload.applicationId,
                payload.configurationId,
                payload.clusterId,
            );
        break;
        default: break;
    }
    return next(action);
}

export default apiMiddleware;