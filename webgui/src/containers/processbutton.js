import { connect } from 'react-redux';
import ProcessButton from '../components/largeprocessbutton';

const mapStateToProps = (state, ownProps) => {
    const processId = ownProps.processId;
    const process = state.model.processes[processId];


    if (!process) {
        return {
            processId,
            applicationName: 'Unknown application',
            clusterName: 'Unkown cluster',
            status: 'Unknown status'
        }
    }

    const status = process.clusterStatus;
    const applicationId = process.applicationId;
    const application = state.model.applications[applicationId];

    const clusterId = process.clusterId;
    const cluster = state.model.clusters[clusterId];

    let applicationName = 'Unknown application';
    let clusterName = 'Unknown cluster';

    if (application) {
        applicationName = application.name;
    }

    if (cluster) {
        clusterName = cluster.name;
    }
   
    return {
        processId,
        applicationName,
        clusterName,
        status
    }
};

export default connect(mapStateToProps)(ProcessButton);
