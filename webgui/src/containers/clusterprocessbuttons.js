import { connect } from 'react-redux';

import React from 'react';
import PropTypes from 'prop-types';
import StopButton from '../containers/stopbutton';
import RestartButton from '../containers/restartbutton';

const ClusterProcessButtons = props => (
    <div>
        {
            // Stop and restart buttons
            props.processIds && props.processIds.map(processId => (
                <div>
                    <StopButton processId={processId}
                                key={'stop-' + processId}
                                appliction />
                    <RestartButton processId={processId}
                                   key={'restart-' + processId}
                                   appliction/>
                </div>
            ))
        }
    </div>
);

ClusterProcessButtons.propTypes = {
    processIds: PropTypes.array.isRequired,
};


const mapStateToProps = (state, ownProps) => {
    const clusterId = ownProps.clusterId;
    const cluster = state.model.clusters[clusterId];

    if (!cluster) {
        return {
            processIds: []
        }
    }

    const processIds = Object.values(state.model.processes).filter(process => {
        return process.clusterId === clusterId;
    }).map(process => {
        return process.id;
    });

    return {
        processIds,
        children: ownProps.children,
    }    
};

export default connect(mapStateToProps)(ClusterProcessButtons);
