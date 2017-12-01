import { connect } from 'react-redux';
import ClusterButton from '../components/largeclusterbutton';

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;
    const cluster = state.model.clusters[clusterId];

    if (!cluster) {
        return {
            available: false,
            clusterId,
            clusterName: 'Unknown',
            clusterStatus: 'No cluster with id "' + clusterId + '"'
        }
    }

    const clusterName = cluster.name;
    const clusterStatus = cluster.connected ? "Connected" : "Disconnected";
    
    return {
        available: cluster.connected,
        clusterId,
        clusterName,
        clusterStatus
    }
};

export default connect(mapStateToProps)(ClusterButton);

