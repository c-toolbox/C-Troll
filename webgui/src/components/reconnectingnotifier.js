import React from 'react';
//import ReactCssTransitionGroup from 'react-addons-css-transition-group';
import PropTypes from 'prop-types';
import './reconnectingnotifier.css';

const ReconnectingNotifier = (props) => {
    if (props.connected) {
        return [];
    }

    let content = null;
    if (props.reconnecting) {
        content = <div className="reconnecting">Connection lost. Reconnecting...</div>
    } else if (props.connecting) {
        content = <div className="reconnecting">Trying to connect...</div>
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
    connected: PropTypes.bool.isRequired,
    reconnecting: PropTypes.bool.isRequired,
    connecting: PropTypes.bool.isRequired,
};


//import api from '../api';

/*
class Content {
    render() {
        if (!api.initialized && api.reconnecting) {
            return <div className="reconnecting">Trying to connect...</div>;
        }
        if (api.reconnecting) {
            return <div className="reconnecting">Connection lost. Reconnecting...</div>;
        }
        return <div className="connected">Connected!</div>;
    }
}

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
