import { InitializeGui,
		 SetProcessStatus,
		 AddProcessLogMessage,
		 SetProcessLogMessageHistory } from '../../actions';

const initialProcessState = {};

const createProcess = (p) => {
	return {
        id: p.processId,
        applicationId: p.applicationId,
        clusterId: p.clusterId,
        configurationId: p.configurationId,
        clusterStatus: p.clusterStatus || 'Unknown',
        clusterStatusTime: p.time,
        nodeStatusHistory: p.nodeStatusHistory || [],
        logMessages: null
	};
}

const initializeGui = (state, action) => {
	const newState = {};
    action.payload.data.processes.forEach((p) => {
        newState[p.processId] = createProcess(p);
	});
	return newState;
}

const setProcessStatus = (state, action) => {
	const newState = {...state};
	const data = action.payload.data;
	const processId = data.processId;
	const processExisted = newState[processId] !== undefined;
	let newProcess = {};

	if (processExisted) {
		newProcess = {...newState[processId]};
		newProcess.clusterStatus = data.clusterStatus;
	} else {
		newProcess = createProcess(data);
	}
	newState[newProcess.id] = newProcess;
	return newState;
}

const addProcessLogMessage = (state, action) => {
	const data = action.payload.data;

	const newState = {
		...state,
		[data.processId]: {
			...state[data.processId],
			logMessages: [
				...state[data.processId].logMessages || [],
				{...data}
			]
		}
	};
	return newState;
}

const setProcessLogMessageHistory = (state, action) => {
	const data = action.payload.data;
	const processId = data.processId;
	const newState = {
		...state,
		[processId]: {
			...state[processId],
			logMessages: data.messages
		}
	};
	return newState;
}

export default (state = initialProcessState, action) => {
    switch (action.type) {
        case InitializeGui:
            return initializeGui(state, action);
        case SetProcessStatus:
            return setProcessStatus(state, action);
        case AddProcessLogMessage:
        	return addProcessLogMessage(state, action);
        case SetProcessLogMessageHistory:
            return setProcessLogMessageHistory(state, action);
       	default: return state;
    }
};