import { connect } from 'react-redux';
import ClusterButton from '../components/largeclusterbutton';

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;
    const cluster = state.model.clusters[clusterId];

    if (!cluster) {
        return {
            clusterId,
            clusterName: 'Unknown',
        }
    }

    const clusterName = cluster.name;
    
    return {
        clusterId,
        clusterName
    }
};

export default connect(mapStateToProps)(ClusterButton);

