import { connect } from 'react-redux';

import React from 'react';
import ClusterDetails from './clusterdetails';
import Breadcrumbs from '../components/breadcrumbs';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';

const Cluster = props => {
    if (props.loading) {
        return null;
    } else if (props.found) {
        return <ClusterDetails clusterId={props.clusterId}/>
    } else {
        return (
            <Breadcrumbs>
                <Link to="clusters">Clusters</Link>
                Not found
            </Breadcrumbs>
        )
    }
};

Cluster.propTypes = {
    loading: PropTypes.bool.isRequired,
    found: PropTypes.bool.isRequired,
    clusterId: PropTypes.string
};

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.match.params.clusterId.replace('>', '/');
    const cluster = state.model.clusters[clusterId];

    if (cluster) {
        return {
            loading: false,
            found: true,
            clusterId
        }
    } else {
        return {
            loading: state.connection.connecting,
            found: false
        }
    }
};

export default connect(mapStateToProps)(Cluster);






/*import React from 'react';
import { Link } from 'react-router-dom';
import api from '../api';
import Breadcrumbs from './breadcrumbs';
import NodeButton from './nodebutton';
import ProcessButton from './processbutton';

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

        const nodes = [];

        if (cluster.nodes) {
            cluster.nodes.forEach(() => {
                nodes.push(<NodeButton node={cluster.node}/>);
            });
        }

        const activeProcesses = [];

        const processes = api.processes.filter((process) => {
            return process.clusterId === cluster.id && process.clusterStatus !== 'Exit';
        });

        processes.forEach((process) => {
            activeProcesses.push(<ProcessButton key={process.id} type="application" process={process}/>);
        });

        if (activeProcesses.length === 0) {
            activeProcesses.push(<span key="empty" className="no-hits">There are no applications running on this cluster.</span>);
        }

        return (
            <div>
                {breadcrumbs}
                <div className="row">
                    <h1>{cluster.name}</h1>
                </div>
                <div className="row">
                    <h2>Active Processes</h2>
                    {activeProcesses}
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
*/