import { InitializeGui } from '../../actions'

const initialApplicationState = {};

const initializeGui = (state, action) => {
	const newState = {};
    action.payload.data.applications.forEach((a) => {
		newState[a.id] = a;
	});
	return newState;
}

export default (state = initialApplicationState, action) => {
    switch (action.type) {
        case InitializeGui:
            return initializeGui(state, action);
       	default: return state;
    }
};