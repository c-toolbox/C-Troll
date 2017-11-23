import React from 'react';
import { connect } from 'react-redux';
import ApplicationDetails from '../components/applicationdetails';

const ApplicationDetailsContainer = props => {

    const clusterButtons = [];
    const configurationButtons = [];

    return (
        <ApplicationDetails tags={props.tags}
                            description={props.description}
                            name={props.name}
                            clusterButtons={clusterButtons}
                            configurationButtons={configurationButtons}>
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

    const name = application.name;
    const tags = application.tags;
    const description = application.description;
    const clusterIds = application.clusters || [];
   
    return {
        name,
        description,
        tags,
        clusterIds
    }
};

export default connect(mapStateToProps)(ApplicationDetailsContainer);

