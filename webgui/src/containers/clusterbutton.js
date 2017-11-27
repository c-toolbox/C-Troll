import { connect } from 'react-redux';
import ClusterButton from '../components/clusterbutton';

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;
    const cluster = state.model.clusters[clusterId];

    if (!cluster) {
        return {
            clusterId,
            clusterName: 'Unknown',
            clusterStatus: ''
        }
    }

    const clusterName = cluster.name;
    const clusterStatus = cluster.connected ? "Connected" : "Disconnected";
    
    return {
        clusterId,
        clusterName,
        clusterStatus
    }
};

export default connect(mapStateToProps)(ClusterButton);

