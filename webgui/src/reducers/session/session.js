import { combineReducers } from 'redux';
import applications from './applications';
import isReloadingConfig from './isreloadingconfig';

export default combineReducers({
    applications,
    isReloadingConfig,
})
