import { InitializeGui } from '../../actions'

const initialClusterState = {};

const initializeGui = (state, action) => {
	const newState = {};
    console.log(action.payload.data.clusters);
    action.payload.data.clusters.forEach((c) => {
		newState[c.id] = c;
	});
    console.log(newState);
	return newState;
}

export default (state = initialClusterState, action) => {
    switch (action.type) {
        case InitializeGui:
            return initializeGui(state, action);
       	default: return state;
    }
};