import React from 'react';
import { connect } from 'react-redux';
import ClusterDetails from '../components/clusterdetails';
import ProcessControlButton from './processcontrolbutton';
import NodeButton from './nodebutton';
import { isProcessActive } from '../query';

const ClusterDetailsContainer = props => {

    const processButtons = props.processIds.map(processId => (
        <ProcessControlButton key={processId} processId={processId}/>
    ));
    const nodeButtons = props.nodeIds.map(nodeId => (
        <NodeButton key={nodeId} clusterId={props.clusterId} nodeId={nodeId}/>
    ));

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

    const processIds = Object.values(state.model.processes).filter(process => {
        return process.clusterId === clusterId;
    }).filter(process => {
        return isProcessActive(state, process.id);
    }).map(process => process.id);


    const nodeIds = [];

    const name = cluster.name;
    const description = cluster.description;
   
    return {
        name,
        description,
        clusterId,
        processIds,
        nodeIds
    }
};

export default connect(mapStateToProps)(ClusterDetailsContainer);

