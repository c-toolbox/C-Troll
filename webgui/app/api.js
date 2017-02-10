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
        const sock = this._sock = new SockJS('http://localhost:3001/ws');
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
                    this.processStatus(data.payload);
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
                this._state.processes.set(p.id, p);
            });
        }

        this._state.reconnecting = false;
        this._state.initialized = true;
        this._state.connected = true;
    }

    processStatus(data) {
        let process = null;
        if (this._state.processes.has(data.processId)) {
            process = this._state.processes.get(data.processId);
        } else {
            process = {
                id: data.processId,
                applicationId: data.applicationId,
                clusterId: data.clusterId,
                configurationId: data.configurationId,
                statuses: []
            };
            this._state.processes.set(data.processId, process);
        }

        if (!process) {
            console.error('Incoming data about unknown process');
        }
        process.statuses.push(data.status);
    }

    sendCommand(data) {
        const message = {
            type: 'GuiCommand',
            payload: data
        };
        this._sock.send(JSON.stringify(message));
    }

    startApplication(applicationId, configurationId, clusterId) {
        console.log('launching application ' + applicationId);

        const data = {
            command: 'Start',
            applicationId: applicationId,
            configurationId: configurationId,
            clusterId: clusterId
        };
        this.sendCommand(data);
    }

    stopProcess(processId) {
        console.log('stopping application' + processId);
    }

    restartProcess(processId) {
        console.log('restarting application' + processId);
    }
}

export default new Api();
