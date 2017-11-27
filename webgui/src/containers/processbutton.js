import { connect } from 'react-redux';
import ProcessButton from '../components/processbutton';

const mapStateToProps = (state, ownProps) => {
    const processId = ownProps.processId;
    const process = state.model.processes[processId];

    if (!process) {
        return {
            processId,
            applicationName: 'Unknown'
        }
    }

    const applicationId = process.applicationId;
    const application = state.model.applications[applicationId];

    if (!application) {
        return {
            processId,
            applicationName: 'Unknown'
        }
    }

    const applicationName = application.name;
    const tags = application.tags;

    
    return {
        processId,
        applicationName,
        tags
    }
};

export default connect(mapStateToProps)(ProcessButton);
