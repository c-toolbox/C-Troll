import SockJS from 'sockjs-client';
import { observable, computed } from 'mobx';
import Map from 'es6-map';

class Api {
    @observable _state = {
        applications: [],
        clusters: [],
        processes: new Map(),
        reconnecting: false,
        initialized: false,
        connected: false,
    }

    @computed get connected() {
        return this._state.connected;
    }

    @computed get initialized() {
        return this._state.initialized;
    }

    @computed get reconnecting() {
        return this._state.reconnecting;
    }

    @computed get applications() {
        return this._state.applications;
    }

    @computed get clusters() {
        return this._state.clusters;
    }

    @computed get processes() {
        return this._state.processes.values();
    }

    tryReconnect() {
        console.log('try reconnect');
        if (this._state.connected) {
            return;
        }
        this._state.reconnecting = true;
        this.initialize();
    }

    initialize() {
        console.log('Connecting to host');
        const sock = this._sock = new SockJS('/api');
        sock.onopen = () => {
            console.log('Connection established to GUI backend.');
        };

        sock.onclose = () => {
            console.log('Connection closed to GUI backend.');
            this._state.connected = false;
            this.tryReconnect();
        };

        sock.onmessage = (message) => {
            const data = JSON.parse(message.data);
            switch (data.type) {
                case 'GuiInit':
                    this.initializeGui(data.payload);
                    break;
                case 'TrayProcessStatus':
                    this.handleProcessStatus(data.payload);
                    break;
                default:
                    console.log('unknown message type: "' + data.type + '"', data.payload);
            }
        };
    }

    initializeGui(data) {
        this._state.applications = data.applications || [];
        this._state.clusters = data.clusters || [];
        this._state.processes.clear();

        if (data.processes) {
            data.processes.forEach((p) => {
                this._state.processes.set(p.id, observable(p));
            });
        }

        this._state.reconnecting = false;
        this._state.initialized = true;
        this._state.connected = true;
    }

    handleProcessStatus(data) {
        let process = null;
        if (this._state.processes.has(data.processId)) {
            process = this._state.processes.get(data.processId);
        } else {
            process = observable({
                id: data.processId,
                applicationId: data.applicationId,
                clusterId: data.clusterId,
                configurationId: data.configurationId,
                clusterStatus: data.clusterStatus,
                clusterStatusTime: data.time,
                nodeStatusHistory: []
            });
            this._state.processes.set(data.processId, process);
        }

        if (!process) {
            console.error('Incoming data about unknown process');
        }
        if (process.clusterStatus !== data.clusterStatus) {
            process.clusterStatus = data.clusterStatus;
            process.clusterStatusTime = data.time;
        }

        Object.keys(data.nodeStatus).forEach((node) => {
            const status = data.nodeStatus[node];
            process.nodeStatusHistory.push({
                time: data.time,
                node: node,
                status: status
            });
        });
    }

    sendCommand(type, data) {
        const message = {
            type: type,
            payload: data
        };
        this._sock.send(JSON.stringify(message));
    }

    startApplication(applicationId, configurationId, clusterId) {
        console.log('launching application ' + applicationId);

        const data = {
            applicationId: applicationId,
            configurationId: configurationId,
            clusterId: clusterId
        };
        this.sendCommand('GuiStartCommand', data);
    }

    stopProcess(processId) {
        console.log('stopping process ' + processId);
        const data = {
            command: 'Stop',
            processId: processId
        };
        this.sendCommand('GuiProcessCommand', data);
    }

    restartProcess(processId) {
        console.log('restarting process ' + processId);
        const data = {
            command: 'Restart',
            processId: processId
        };
        this.sendCommand('GuiProcessCommand', data);
    }
}

export default new Api();
