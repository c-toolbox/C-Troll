import { InitializeGui } from '../../actions'

const initialClusterState = {};

const initializeGui = (state, action) => {
	const newState = {};
    action.payload.data.clusters.forEach((c) => {
		newState[c.id] = c;
	});
	return newState;
}

export default (state = initialClusterState, action) => {
    switch (action.type) {
        case InitializeGui:
            return initializeGui(state, action);
       	default: return state;
    }
};