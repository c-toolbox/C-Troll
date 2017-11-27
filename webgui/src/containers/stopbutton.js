import { connect } from 'react-redux';
import StopButton from '../components/stopbutton';
import { stopProcess } from '../actions';
import { applicationNameByProcessId,
         clusterNameByProcessId } from '../query';

const mapStateToProps = (state, ownProps) => {
    const processId = ownProps.processId;

    const props = {
        enabled: state.connection.connected
    };

    if (ownProps.application) {
        props.application = applicationNameByProcessId(state, processId);
    } 
    if (ownProps.cluster) {
        props.cluster = clusterNameByProcessId(state, processId);
    }

    return props;
};

const mapDispatchToProps = (dispatch, ownProps) => {
    const onClick = () => {
        dispatch(stopProcess(ownProps.processId));
    }

    return {
        onClick
    }
}

export default connect(mapStateToProps, mapDispatchToProps)(StopButton);