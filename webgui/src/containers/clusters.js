import React from 'react';
import { connect } from 'react-redux';
import ClusterButton from './clusterbutton';
import ClusterProcessButtons from './clusterprocessbuttons';
import List from '../components/list';

const getClusterId = (cluster) => {
    return cluster.id;
}

const getClusterIds = state => {
    const allClusters = Object.values(state.model.clusters);
    return allClusters.map(getClusterId);
}

const mapStateToProps = (state) => {
    const clusterIds = getClusterIds(state);
    return {
        clusterIds
    };
}

const Clusters = (props) => {
    return (
        <div>
            <div className="row"><h2>Clusters</h2></div>
            <List>
                {
                    props.clusterIds.map((clusterId) => (
                        <ClusterButton clusterId={clusterId}
                                       key={clusterId}>
                            <ClusterProcessButtons clusterId={clusterId}/>
                        </ClusterButton>
                    ))
                }
            </List>
        </div>
    )
};


/*
class Clusters extends React.Component {
    render() {
        return (
            <div>
                <div className="row">
                    <h2>Clusters</h2>
                </div>
                <div className="row button-container">
                    {
                        api.clusters.map((cluster) => {
                            const contents = [];
                            const clusterProcesses = api.processes.filter((process) => {
                                return process.clusterId === cluster.id && process.clusterStatus !== 'Exit';
                            });

                            clusterProcesses.forEach((process) => {
                                contents.unshift(<StopButton key={'stop' + process.id} type="application" process={process}/>);
                                contents.unshift(<RestartButton key={'restart#' + process.id} type="application" process={process}/>);
                            });

                            return <ClusterButton key={cluster.id} cluster={cluster}>{contents}</ClusterButton>;
                        })
                    }
                </div>
            </div>
        );
    }
}*/


export default connect(mapStateToProps)(Clusters);
