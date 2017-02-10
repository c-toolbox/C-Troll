import React from 'react';
import ReactCssTransitionGroup from 'react-addons-css-transition-group';

import api from '../api';
import { observer } from 'mobx-react';

@observer
class Content extends React.Component {
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

@observer
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

export default ReconnectingNotifier;
