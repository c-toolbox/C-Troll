import React from 'react';
import { observer } from 'mobx-react';
import { Link } from 'react-router';
import api from '../api';
import Breadcrumbs from './breadcrumbs';
import NodeButton from './nodebutton';

@observer
class Cluster extends React.Component {
    constructor({ params: { clusterId }}) {
        super();
        this._clusterId = clusterId.replace('>', '/');
    }

    render() {
        const cluster = api.clusters.find((c) => {
            return c.id === this._clusterId;
        });

        const pageName = cluster ? (<span>{cluster.name}</span>) : (<span>{this._clusterId}</span>);

        const breadcrumbs = (
            <Breadcrumbs>
                <Link to="/clusters">Clusters</Link>
                {pageName}
            </Breadcrumbs>
        );

        if (cluster === undefined) {
            return breadcrumbs;
        }

        let nodes = [];

        if (cluster.nodes) {
            cluster.nodes.forEach(() => {
                nodes.push(<NodeButton node={cluster.node}/>);
            });
        }

        return (
            <div>
                {breadcrumbs}
                <div className="row">
                    <h1>{cluster.name}</h1>
                </div>
                <div className="row">
                    No applications running
                </div>
                <div className="row">
                    <h2>Actions</h2>
                </div>
                <div className="row">
                    <h2>Nodes</h2>
                    {nodes}
                </div>
            </div>
        );
    }
}

export default Cluster;
