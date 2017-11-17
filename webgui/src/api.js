import SockJS from 'sockjs-client';
import Map from 'es6-map';

class Api {
   _state = {
        applications: [],
        clusters: [],
        processes: new Map(),
        reconnecting: false,
        initialized: false,
        connected: false,
    }

    get connected() {
        return this._state.connected;
    }

    get initialized() {
        return this._state.initialized;
    }

    get reconnecting() {
        return this._state.reconnecting;
    }

    get applications() {
        return [] // this._state.applications;
    }

    get clusters() {
        return []; //this._state.clusters;
    }

    get processes() {
        return [] //this._state.processes.values();
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
                case 'GuiProcessStatus':
                    this.handleProcessStatus(data.payload);
                    break;
                case 'GuiProcessLogMessage':
                    this.handleProcessLogMessage(data.payload);
                    break;
                case 'GuiProcessLogMessageHistory':
                    this.handleProcessLogMessageHistory(data.payload);
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
                this.getOrCreateProcess(p);
            });
        }

        this._state.reconnecting = false;
        this._state.initialized = true;
        this._state.connected = true;
    }

    /**
     * Get or create process based on data object with keys
     * processId
     * applicationId
     * clusterId
     * configurationId
     * clusterStatus
     */
    getOrCreateProcess(data) {
        if (this._state.processes.has(data.processId)) {
            return this._state.processes.get(data.processId);
        }
        const process = {
            id: data.processId,
            applicationId: data.applicationId,
            clusterId: data.clusterId,
            configurationId: data.configurationId,
            clusterStatus: data.clusterStatus || 'Unknown',
            clusterStatusTime: data.time,
            nodeStatusHistory: data.nodeStatusHistory || [],
            logMessages: null
        };
        this._state.processes.set(data.processId, process);
        return process;
    }

    handleProcessStatus(data) {
        const process = this.getOrCreateProcess(data);

        if (process.clusterStatus !== data.clusterStatus) {
            process.clusterStatus = data.clusterStatus;
            process.clusterStatusTime = data.time;
        }

        Object.keys(data.nodeStatus).forEach((node) => {
            const status = data.nodeStatus[node];
            process.nodeStatusHistory.push({
                id: data.id,
                time: data.time,
                nodeId: node,
                status: status
            });
        });
    }

    handleProcessLogMessage(data) {
        const process = this.getOrCreateProcess(data);

        if (!process.logMessages) {
            process.logMessages = [];
        }

        process.logMessages.push({
            id: data.id,
            nodeId: data.nodeId,
            time: data.time,
            outputType: data.outputType,
            message: data.message
        });
    }

    handleProcessLogMessageHistory(data) {
        const process = this.getOrCreateProcess(data);
        process.logMessages = data.messages;
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
