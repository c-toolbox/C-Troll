import { connect } from 'react-redux';
import ReconnectingNotifier from '../components/reconnectingnotifier'

const mapStateToProps = state => {
    return {
        connected: state.connection.connected || false,
        connecting: state.connection.connecting || false,
        reconnecting: state.connection.connectionLost && state.connection.connecting
    }
};

export default connect(mapStateToProps)(ReconnectingNotifier);