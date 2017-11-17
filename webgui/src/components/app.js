import React from 'react';
import PropTypes from 'prop-types';
import { Link, BrowserRouter, Route } from 'react-router-dom';
import api from '../api';
import ReconnectingNotifier from './reconnectingnotifier';

import ApplicationsIcon from './applicationsicon';
import ClustersIcon from './clustersicon';

import Applications from './applications';
import Application from './application';
import Clusters from './clusters';
import Cluster from './cluster';
import Process from './process';

import './main.css';

class App extends React.Component {
    componentDidMount() {
        api.initialize();
    }

    render() {
        const clustersActive = false;
            //this.context.router.location.pathname.indexOf('/clusters') === 0 ||
            //this.context.router.location.pathname.indexOf('/processes') === 0;
        const appsActive = !clustersActive;

        return (
            <BrowserRouter>
                <div>
                    <ReconnectingNotifier/>
                    <div>
                        <nav id="side-bar">
                            <Link className={appsActive ? 'active' : ''} to="/applications">
                                <ApplicationsIcon/>
                                Applications
                            </Link>
                            <Link className={clustersActive ? 'active' : ''} to="/clusters">
                                <ClustersIcon/>
                                Clusters
                            </Link>
                        </nav>
                        <div id="main-container">
                            <Route exact path="/" component={Applications}/>
                            <Route path="/applications" component={Applications} />
                            <Route path="/applications/:appId" component={Application}/>
                            <Route path="/clusters" component={Clusters} />
                            <Route path="/clusters/:clusterId" component={Cluster} />
                            <Route path="/processes/:processId" component={Process}/>
                            <Route path="/tags/:tag" component={Applications}/>
                        </div>
                    </div>
                </div>
            </BrowserRouter>
        );
    }
}

export default App;
