import React from 'react';
import { connect } from 'react-redux';
import ApplicationDetails from '../components/applicationdetails';
import ApplicationClusterButtons from './applicationclusterbuttons';

const ApplicationDetailsContainer = props => {

    const clusterButtons = <ApplicationClusterButtons applicationId={props.id} />;
    const configurationButtons = []; //<ApplicationConfigurationButtons applicationId={props.id} />;

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

    const id = application.id;
    const name = application.name;
    const tags = application.tags;
    const description = application.description;
    const clusterIds = application.clusters || [];
   
    return {
        id,
        name,
        description,
        tags,
        clusterIds
    }
};

export default connect(mapStateToProps)(ApplicationDetailsContainer);

