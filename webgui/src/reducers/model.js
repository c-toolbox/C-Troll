import {
    InitializeGui,
    StartProcess,
} from '../actions';

const initialModelState = {
    applications: {},
    clusters: {},
    processes: {}
};

// Initialize gui reducer
function initializeGui(state, action) {
    const data = action.payload.data;

    const newState = {...state};

    newState.applications = {};
    newState.clusters = {};
    newState.processes = {};

    if (data.applications) {
    	data.applications.forEach((a) => {
    		newState.applications[a.id] = a;
    	});
    }

    if (data.clusters) {
    	data.clusters.forEach((c) => {
    		newState.clusters[c.id] = c;
    	})
    }

    if (data.processes) {
        data.processes.forEach((p) => {
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
            state.processes[data.processId] = process;
        });
    }

    newState.initialized = true;
    return newState;
}

function startProcess(state, action) {
	return state;
}

export default (state = initialModelState, action) => {
    switch (action.type) {
        case InitializeGui:
            return initializeGui(state, action);
        case StartProcess:
        	return startProcess(state, action);
       	default: return state;
    }
};
