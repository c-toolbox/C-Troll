import { combineReducers } from 'redux';
import model from './model/model';
import connection from './connection/connection';
import session from './session/session'

export default combineReducers({
    model,
    connection,
    session,
})
