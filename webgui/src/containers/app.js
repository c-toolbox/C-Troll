import React from 'react';
import { BrowserRouter } from 'react-router-dom';
import store from '../store';
import { Provider } from 'react-redux';

import ReconnectingNotifier from './reconnectingnotifier';
import MainWindow from './mainwindow';

import MainNavivation from '../containers/mainnavigation';

import '../components/main.css';

class App extends React.Component {
    render() {
        return (
            <Provider store={store}>
                <BrowserRouter>
                    <div>
                        <ReconnectingNotifier/>
                        <div>
                            <MainNavivation/>
                            <MainWindow/>
                        </div>
                    </div>
                </BrowserRouter>
            </Provider>
        );
    }
}

export default App;