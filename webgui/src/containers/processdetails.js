import { connect } from 'react-redux';
import ProcessDetails from '../components/processdetails';

const mapStateToProps = (state, ownProps) => {
    const processId = ownProps.processId;
    const process = state.model.processes[processId];

    if (!process) {
        return {
            applicationName: 'Unknown application',
            clusterName: 'Unknown cluster',
            clusterStatus: 'Unknown status'
        }
    }

    const application = state.model.applications[process.applicationId];
    const cluster = state.model.clusters[process.clusterId];
    const logMessages = process.logMessages;
    const nodeStatusHistory = process.nodeStatusHistory;

    return {
        applicationName: application.name,
        clusterName: cluster.name,
        clusterStatus: process.clusterStatus,
        logMessages,
        nodeStatusHistory
    }
};

export default connect(mapStateToProps)(ProcessDetails);

