import React from 'react';
import { connect } from 'react-redux';
import ClusterDetails from '../components/clusterdetails';

const ClusterDetailsContainer = props => {

    const processButtons = [];
    const nodeButtons = [];

    return (
        <ClusterDetails name={props.name}
                        description={props.description}
                        processButtons={processButtons}
                        nodeButtons={nodeButtons}>
        </ClusterDetails>
    );
}

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;
    const cluster = state.model.clusters[clusterId];

    if (!cluster) {
        return {
            name: ""
        }
    }


    const name = cluster.name;
    const description = cluster.description;
   
    return {
        name,
        description
    }
};

export default connect(mapStateToProps)(ClusterDetailsContainer);

