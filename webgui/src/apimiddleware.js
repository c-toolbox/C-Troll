import SockJS from 'sockjs-client';
import { 
    ServerConnect,
    StartProcess,
    RestartProcess,
    StopProcess,
    ReloadConfig,
    ServerDisconnected,
    initializeGui,
    setProcessStatus,
    addProcessLogMessage,
    setProcessLogMessageHistory,
    serverConnected,
    serverDisconnected } from './actions';

import deepFreeze from 'deep-freeze';

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

function sendReloadConfigCommand() {
    sendCommand('GuiReloadConfigCommand', {});
}

function scheduleReconnection(dispatch) {
    setTimeout(() => {serverConnect(dispatch)}, 2000);
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
        deepFreeze(data);
        switch (data.type) {
            case 'GuiInit':
                dispatch(initializeGui(data.payload));
                break;
            case 'GuiProcessStatus':
                dispatch(setProcessStatus(data.payload));
                break;
            case 'GuiProcessLogMessage':
                dispatch(addProcessLogMessage(data.payload));
                break;
            case 'GuiProcessLogMessageHistory':
                dispatch(setProcessLogMessageHistory(data.payload));
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
                payload.processId
            );
        break;
        case StopProcess:
            sendStopProcessCommand(
                payload.processId
            );
        break;
        case ServerDisconnected:
            scheduleReconnection(store.dispatch);
            break;
        case ReloadConfig:
            sendReloadConfigCommand();
        break;
        default: break;
    }
    return next(action);
}

export default apiMiddleware;