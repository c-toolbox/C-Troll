import React from 'react';
import PropTypes from 'prop-types';

const icon = (<svg className="stop-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
    <rect className="stop-icon-rect" x="0" y="0" width="64" height="64"/>
</svg>);

const StopButton = props => {
    const classNames = ['stop-button', 'no-select', 'sub-button'];
    if (props.enabled === false || !props.onClick) {
        classNames.push('inactive');
    }

    const stop = (evt) => {
        if (!props.enabled) {
            return;
        }
        props.onClick && props.onClick()
        evt.stopPropagation();
    }

    let text = null;
    if (props.application && props.cluster) {
        text = (<span>{props.application} on {props.cluster}</span>);
    } else if (props.application) {
        text = (<span>{props.application}</span>);
    } else if (props.cluster) {
        text = (<span>{props.cluster}</span>);
    }

    return (<div onClick={stop} className={classNames.join(' ')}>{icon}{text}</div>);
}

StopButton.propTypes = {
    cluster: PropTypes.string,
    application: PropTypes.string,
    onClick: PropTypes.func.isRequired,
    enabled: PropTypes.bool
};

export default StopButton;