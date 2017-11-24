import { connect } from 'react-redux';
import StartButton from '../components/startbutton';
import { startProcess } from '../actions';

const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.applicationId;
    const clusterId = ownProps.clusterId;
    const configurationId = ownProps.configurationId;

    const clusterConnected = state.model.clusters[clusterId] &&
        state.model.clusters[clusterId].connected;

    const props = {
        enabled: state.connection.connected && clusterConnected
    };

    if (ownProps.application) {
        props.application = state.model.applications[applicationId].name;
    } 
    if (ownProps.cluster) {
        props.cluster = state.model.clusters[clusterId].name;
    }

    props.startButtons = [
        {
            configurationId
        }
    ]

    return props;
};

const mapDispatchToProps = (dispatch, ownProps) => {
    const onClick = () => {
        dispatch(
            startProcess(ownProps.applicationId,
                         ownProps.configurationId,
                         ownProps.clusterId)
            );
    }

    return {
        onClick
    }
}

export default connect(mapStateToProps, mapDispatchToProps)(StartButton);