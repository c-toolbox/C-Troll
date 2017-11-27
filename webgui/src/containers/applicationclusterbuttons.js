import React from 'react';
import { connect } from 'react-redux';
import ClusterButton from './clusterbutton';
import ClusterProcessButtons from './clusterprocessbuttons';
import StartButton from './startbutton';
import List from '../components/list';

const getConfigurationId = (state, applicationId) => {
    return state.model.applications[applicationId].configurations[0] ? 
            state.model.applications[applicationId].configurations[0].id :
            '';
}

const getClusterIds = (state, applicationId) => {
    const application = state.model.applications[applicationId];
    if (!application) {
        return [];
    }

    return application.clusters;
}

const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.applicationId;
    const clusterIds = getClusterIds(state, applicationId);
    const configurationId = getConfigurationId(state, applicationId);
    return {
        clusterIds,
        applicationId,
        configurationId
    };
}

const mapDispatchToProps = (dispatch) => {
    return {
        startApplication: function() {}
    }
}

const ApplicationClusterButtons = (props) => {
    return (
        <List>
            {
                props.clusterIds.map((clusterId) => (
                    <ClusterButton clusterId={clusterId}
                                   key={clusterId}>
                        <ClusterProcessButtons clusterId={clusterId}/>
                        <StartButton application applicationId={props.applicationId} clusterId={clusterId} configurationId={props.configurationId}/>
                    </ClusterButton>
                ))
            }
        </List>
    )
};




export default connect(mapStateToProps, mapDispatchToProps)(ApplicationClusterButtons);
