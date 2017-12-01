import { connect } from 'react-redux';

import React from 'react';
import PropTypes from 'prop-types';
import StopButton from '../containers/stopbutton';
import RestartButton from '../containers/restartbutton';

import { isProcessActive } from '../query';

const ClusterProcessButtons = props => (
    <div>
        {
            // Stop and restart buttons
            props.processIds && props.processIds.map(processId => (
                <div key={processId}>
                    <StopButton processId={processId}
                                application />
                    <RestartButton processId={processId}
                                   application/>
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
    }).filter(process => {
        return isProcessActive(state, process.id)
    }).map(process => {
        return process.id;
    });

    return {
        processIds,
        children: ownProps.children,
    }    
};

export default connect(mapStateToProps)(ClusterProcessButtons);
