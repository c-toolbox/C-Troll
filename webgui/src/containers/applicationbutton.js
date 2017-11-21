import { connect } from 'react-redux';
import ApplicationButton from '../components/applicationbutton'

const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.applicationId;
    const application = state.model.applications[applicationId];

    if (!application) {
        return {
            applicationId,
            applicationName: 'Unknown',
            tags: []
        }
    }

    const applicationName = application.name;
    const tags = application.tags;


    return {
        applicationId,
        applicationName,
        tags
    }
};

export default connect(mapStateToProps)(ApplicationButton);


/*

const buttons = [];

        if (application.clusters.length > 0) {
            const clusterId = application.clusters[0];
            const cluster = api.clusters.find((c) => {
                return c.id === clusterId;
            });
            if (cluster) {
                buttons.push((<StartButton key="start" type="cluster" application={application} cluster={cluster}/>));
            } else {
                buttons.push(<MoreButton key="disabled"/>);
            }
        } else {
            buttons.push(<MoreButton key="disabled"/>);
        }

        buttons.push(<MoreButton to={appUrl} key="more"/>);
*/