import { connect } from 'react-redux';
import ReconnectingNotifier from '../components/reconnectingnotifier'

const mapStateToProps = state => {
    return {
        initialized: state.connection.initialized || false,
        connected: state.connection.connected || false,
        connecting: state.connection.connecting || false,
        reconnecting: state.connection.connectionLost
    }
};

export default connect(mapStateToProps)(ReconnectingNotifier);