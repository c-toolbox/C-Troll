import React from 'react';
import { BrowserRouter, Route } from 'react-router-dom';
import store from '../store';
import ReconnectingNotifier from './reconnectingnotifier';
import { Provider } from 'react-redux';
import Applications from './applications';

//import ApplicationsIcon from './applicationsicon';
//import ClustersIcon from './clustersicon';

//import Applications from './applications';
//import Application from './application';
//import Clusters from './clusters';
//import Cluster from './cluster';
//import Process from './process';

import MainNavivation from './mainnavigation';

import '../components/main.css';

class App extends React.Component {
    render() {
        //const clustersActive = false;
            //this.context.router.location.pathname.indexOf('/clusters') === 0 ||
            //this.context.router.location.pathname.indexOf('/processes') === 0;
        //const appsActive = !clustersActive;

        return (
            <Provider store={store}>
                <BrowserRouter>
                    <div>
                        <ReconnectingNotifier/>
                        <div>
                            <MainNavivation/>
                            <div id="main-container">
                                <Route exact path="/" component={Applications}/>
                                <Route path="/applications" component={Applications} />
                            </div>
                        </div>
                    </div>
                </BrowserRouter>
            </Provider>
        );
    }
}

export default App;


/*
<Route exact path="/" component={Applications}/>
                            
                            <Route path="/applications/:appId" component={Application}/>
                            <Route path="/clusters" component={Clusters} />
                            <Route path="/clusters/:clusterId" component={Cluster} />
                            <Route path="/processes/:processId" component={Process}/>
                            <Route path="/tags/:tag" component={Applications}/>

*/