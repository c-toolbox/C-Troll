import React from 'react';
import { IndexRoute, Route } from 'react-router';
import App from './components/app';

import Applications from './components/applications';
import Application from './components/application';
import Clusters from './components/clusters';
import Cluster from './components/cluster';
import Process from './components/process';

export default (
	<Route path="/" component={App}>
		<IndexRoute component={Applications}/>
		<Route path="/applications" component={Applications} />
		<Route path="/applications/:appId" component={Application}/>
		<Route path="/clusters" component={Clusters} />
		<Route path="/clusters/:clusterId" component={Cluster} />
		<Route path="/processes/:processId" component={Process}/>
		<Route path="/tags/:tag" component={Applications}/>
	</Route>
);
