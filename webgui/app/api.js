import SockJS from 'sockjs-client';
import { observable, computed } from 'mobx';

class Api {
    @observable _state = {
        applications: [],
        clusters: [],
        processes: [],
        loading: true
    }

    @computed get applications() {
        return this._state.applications;
    }

    @computed get clusters() {
        return this._state.clusters;
    }

    @computed get processes() {
        return this._state.processes;
    }

    @computed get loading() {
        return this._state.loading;
    }

    initialize() {
        const sock = this._sock = new SockJS('http://localhost:3001/ws');
        sock.onopen = () => {
            console.log('Connection established to GUI backend.');
        };

        sock.onclose = () => {
            console.log('Connection closed to GUI backend.');
        };

        sock.onmessage = (message) => {
            const data = JSON.parse(message.data);
            switch (data.type) {
                case 'GuiInit':
                    this.initializeGui(data.payload);
                    break;
                default:
                    console.log('unknown message type: "' + data.type + '"', data.payload);
            }
        };
    }

    initializeGui(data) {
        this._state.applications = data.applications || [];
        this._state.clusters = data.clusters || [];
        this._state.processes = data.processes || [];
        this._state.loading = false;
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

    quitApplication(applicationId) {
        console.log('quitting application ' + applicationId);
    }
}

export default new Api();
