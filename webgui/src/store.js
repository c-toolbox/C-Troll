import { serverConnect } from './actions'
import { createStore, applyMiddleware } from 'redux';
import apiMiddleware from './apimiddleware';
import reducer from './reducers/root';

const store = createStore(
  reducer,
  applyMiddleware(
    apiMiddleware
  )
);

store.dispatch(serverConnect());

export default store;

/*
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

*/

//export default new Api();
