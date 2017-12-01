import React from 'react';
//import ReactCssTransitionGroup from 'react-addons-css-transition-group';
import PropTypes from 'prop-types';
import './reconnectingnotifier.css';

const ReconnectingNotifier = (props) => {
    if (props.connected && props.initialized) {
        return null;
    }

    let content = null;
    if (props.reconnecting || !props.initialized) {
        content = <div className="reconnecting">Connection lost. Reconnecting...</div>
    } else if (props.connecting || !props.initialized) {
        content = <div className="reconnecting">Connecting...</div>
    } else {
        content = <div className="disconnected">Disconnected</div>
    }

    return (
        <div className="reconnecting-notifier">
            {content}
        </div>
    );
};

ReconnectingNotifier.propTypes = {
    initialized: PropTypes.bool.isRequired,
    connected: PropTypes.bool.isRequired,
    reconnecting: PropTypes.bool.isRequired,
    connecting: PropTypes.bool.isRequired,
};


/*
class ReconnectingNotifier extends React.Component {
    render() {
        const items = [];
        if (api.reconnecting) {
            items.push(<div className="reconnecting-notifier" key="0">
                <Content/>
            </div>);
        }
        return (
            <ReactCssTransitionGroup
                transitionName="reconnecting-notifier"
                transitionEnterTimeout={400}
                transitionLeaveTimeout={2400}
                component="div">
                {items}
            </ReactCssTransitionGroup>
        );
    }
}
*/
export default ReconnectingNotifier;
