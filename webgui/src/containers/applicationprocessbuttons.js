import { connect } from 'react-redux';

import React from 'react';
import PropTypes from 'prop-types';
import StopButton from '../containers/stopbutton';
import RestartButton from '../containers/restartbutton';
import StartButton from '../containers/startbutton';

const ApplicationProcessButtons = props => (
    <div>
        {
            // Start buttons
            props.startButtons && props.startButtons.map(startButton => {
                const key = 'start-' + startButton.applicationId +
                    '-' + startButton.clusterId + 
                    '-' + startButton.configutarionId;
                return (
                    <StartButton key={key}
                                 applicationId={startButton.applicationId}
                                 clusterId={startButton.clusterId}
                                 configutarionId={startButton.configurationId}
                                 cluster/>
                );
            })
        }
        {
            // Stop and restart buttons
            props.processIds && props.processIds.map(processId => (
                <div>
                    <StopButton processId={processId}
                                key={'stop-' + processId}
                                cluster />
                    <RestartButton processId={processId}
                                   key={'restart-' + processId}
                                   cluster/>
                </div>
            ))
        }
    </div>
);

ApplicationProcessButtons.propTypes = {
    processIds: PropTypes.array.isRequired,
    startButtons: PropTypes.array
};


const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.applicationId;
    const application = state.model.applications[applicationId];
    const defaultStartButton = (ownProps.startButtons === 'default');

    if (!application) {
        return {
            processIds: []
        }
    }

    const processIds = Object.values(state.model.processes).filter(process => {
        return process.applicationId === applicationId;
    }).map(process => {
        return process.id;
    });

    const startButtons = [];
    if (defaultStartButton && application.clusters.length > 0) {
        const clusterId = application.clusters[0];
        startButtons.push({
            applicationId: applicationId,
            configurationId: 0,
            clusterId: clusterId
        });
    }

    return {
        processIds,
        children: ownProps.children,
        startButtons
    }    
};

export default connect(mapStateToProps)(ApplicationProcessButtons);
