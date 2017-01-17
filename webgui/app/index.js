import 'babel-polyfill';
import React from 'react';
import { render } from 'react-dom';
import { browserHistory } from 'react-router';
import { AppContainer } from 'react-hot-loader';
import Root from './rootcontainer';

import './styles/main.scss';

render(
    <AppContainer>
        <Root history={browserHistory} />
    </AppContainer>,
    document.getElementById('root')
);

if (module.hot) {
    module.hot.accept('./rootcontainer', () => {
        const NewRoot = require('./rootcontainer').default;
        render(
            <AppContainer>
                <NewRoot history={browserHistory} />
            </AppContainer>,
            document.getElementById('root')
        );
    });
}
