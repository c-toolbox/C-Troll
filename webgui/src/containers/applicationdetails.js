import React from 'react';
import { connect } from 'react-redux';
import ApplicationDetails from '../components/applicationdetails';
//import ApplicationConfigurationButtons from './applicationconfigurationbuttons';
import ConfigurationButton from '../components/configurationbutton';
import { defaultApplicationConfiguration } from '../query';
import { setApplicationConfiguration } from '../actions';

import ClusterButton from './clusterbutton';
import ClusterProcessButtons from './clusterprocessbuttons';
import StartButton from './startbutton';

//import ApplicationProcessButtons from './applicationprocessbuttons';

const ApplicationDetailsContainer = props => {
    const applicationId = props.applicationId;

    const configurationButtons = props.configurations && props.configurations.map((config) => (
        <ConfigurationButton configurationName={config.name}
                             configurationId={config.id}
                             selected={config.selected}
                             key={config.id}
                             onSelect={props.setConfiguration}>
        </ConfigurationButton>
    ));

    const processButtons = [];

    const clusterButtons = props.clusterIds.map((clusterId) => (
        <ClusterButton clusterId={clusterId}
                       key={clusterId}>
            <ClusterProcessButtons clusterId={clusterId}/>
            <StartButton application applicationId={applicationId} configurationId={props.selectedConfiguration} clusterId={clusterId} />
        </ClusterButton>
    ))


    return (
        <ApplicationDetails tags={props.tags}
                            description={props.description}
                            name={props.name}
                            processButtons={processButtons}
                            configurationButtons={configurationButtons}
                            clusterButtons={clusterButtons}>
        </ApplicationDetails>
    );
}

const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.applicationId;
    const application = state.model.applications[applicationId];

    if (!application) {
        return {
            name: "",
            tags: []
        }
    }

    const id = application.id;
    const name = application.name;
    const tags = application.tags;
    const description = application.description;

    const selectedConfiguration =
        state.session.applications.selectedConfigurations[applicationId] ||
        defaultApplicationConfiguration(state, applicationId);

    const configurationIds = Object.keys(application.configurations);
    const configurations = configurationIds.length > 1 && configurationIds.map(k => {
        return {
            id: k,
            name: application.configurations[k].name,
            selected: selectedConfiguration === k
        }
    });

    const clusterIds = application.configurations[selectedConfiguration].clusters;
   

    return {
        id,
        name,
        description,
        tags,
        clusterIds,
        configurations,
        selectedConfiguration
    }
};

const mapDispatchToProps = (dispatch, ownProps) => {
    return {
        setConfiguration: configurationId => {
            dispatch(setApplicationConfiguration(ownProps.applicationId, configurationId));
        }
    }
}

export default connect(mapStateToProps, mapDispatchToProps)(ApplicationDetailsContainer);

