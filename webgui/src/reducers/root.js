import { combineReducers } from 'redux';
import model from './model';
import connection from './connection';
import session from './session'

export default combineReducers({
    model,
    connection,
    session,
})
