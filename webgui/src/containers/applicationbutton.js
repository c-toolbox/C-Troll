import { connect } from 'react-redux';
import ApplicationButton from '../components/largeapplicationbutton';

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
