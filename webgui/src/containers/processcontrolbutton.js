import { connect } from 'react-redux';

import React from 'react';
import PropTypes from 'prop-types';
import ProcessButton from './processbutton';
import StopButton from './stopbutton';
import RestartButton from './restartbutton';

import { isProcessActive } from '../query';

const children = processId => (
    <div>
        <StopButton processId={processId}
                    application />
        <RestartButton processId={processId}
                   application/>
    </div>
);

const ProcessControlButton = props => {
    return (
        <ProcessButton processId={props.processId}>
        { props.active && children(props.processId)}
    </ProcessButton>);
}

const mapStateToProps = (state, ownProps) => {
    return {
        active: isProcessActive(state, ownProps.processId)
    }
}

ProcessControlButton.propTypes = {
    processId: PropTypes.number.isRequired,
};


export default connect(mapStateToProps)(ProcessControlButton);
