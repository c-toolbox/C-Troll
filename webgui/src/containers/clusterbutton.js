import { connect } from 'react-redux';
import ClusterButton from '../components/clusterbutton';

const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;

    console.log(state.model.clusters);
    console.log(clusterId);

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

