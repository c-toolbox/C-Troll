import { InitializeGui } from '../../actions';
import { ReloadConfig } from '../../actions';

const initialReloadingState = false;

export default (state = initialReloadingState, action) => {
    switch (action.type) {
        case InitializeGui:
            return false;
        case ReloadConfig:
            return true;
       	default: return state;
    }
};