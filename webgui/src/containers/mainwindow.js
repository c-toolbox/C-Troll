import React from 'react';
import { Route } from 'react-router-dom';

import Applications from './applications';
import Application from './application';
import Clusters from './clusters';

//import Cluster from './cluster';
//import Process from './process';


const MainWindow = () => {
	return (
		<div id="main-container">
		    <Route exact path="/" component={Applications}/>
		    <Route path="/applications" component={Applications} />
		    <Route path="/application/:applicationId" component={Application}/>
		    <Route path="/clusters" component={Clusters} />
		</div>
	)

}

export default MainWindow;

/*
<Route path="/clusters" component={Clusters} />
<Route path="/clusters/:clusterId" component={Cluster} />
<Route path="/processes/:processId" component={Process}/>
<Route path="/tags/:tag" component={Applications}/>
*/