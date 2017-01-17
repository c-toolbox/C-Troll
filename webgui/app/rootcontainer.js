import React, { Component, PropTypes } from 'react';
import { Router } from 'react-router';
import routes from './routes';

export default class Root extends Component {
    render() {
        const { history } = this.props;
        return (
            <div>
                <Router history={history} routes={routes} />
            </div>
        );
    }
}

Root.propTypes = {
    history: PropTypes.object.isRequired
};
