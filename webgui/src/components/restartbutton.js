import React from 'react';
import PropTypes from 'prop-types';

const icon = (<svg className="restart-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 42 47">
    <path className="restart-icon-path" d="M31.28,18.16A16.33,16.33,0,1,0,43.2,23.32" transform="translate(-10.22 -9.14)"/>
    <path className="restart-icon-arrow" d="M42.72,18.16c-5.38-2-12.06-5.4-16.2-9l3.26,9-3.26,9C30.66,23.56,37.34,20.15,42.72,18.16Z" transform="translate(-10.22 -9.14)"/>
</svg>);

const RestartButton = props => {
    const classNames = ['restart-button', 'no-select', 'sub-button'];
    if (props.enabled === false) {
        classNames.push('inactive');
    }

    const restart = (evt) => {
        props.onClick()
        evt.stopPropagation();
    }

    let text = null;
    if (props.applicaiton && props.cluster) {
        text = (<span>{props.application} on {props.cluster}</span>);
    } else if (props.applicaiton) {
        text = (<span>{props.application}</span>);
    } else if (props.cluster) {
        text = (<span>{props.cluster}</span>);
    }

    return (<div onClick={restart} className={classNames.join(' ')}>{icon}{text}</div>);
}

StopButton.propTypes = {
    cluster: PropTypes.string,
    application: PropTypes.string,
    onClick: PropTypes.func.isRequired,
    enabled: PropTypes.bool
};

export default RestartButton;