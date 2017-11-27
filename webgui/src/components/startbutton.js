import React from 'react';
import PropTypes from 'prop-types';

const icon = (<svg className="start-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
    <path className="start-icon-path" d="M61.83,35.49L5.65,63.58A3.91,3.91,0,0,1,0,60.09V3.91A3.91,3.91,0,0,1,5.65.42L61.83,28.51A3.91,3.91,0,0,1,61.83,35.49Z"/>
</svg>);

const StartButton = props => {
    const classNames = ['start-button', 'no-select', 'sub-button'];
    if (props.enabled === false || !props.onClick) {
        classNames.push('inactive');
    }

    const start = (evt) => {
        if (!props.enabled) {
            return;
        }
        props.onClick && props.onClick();
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

    return (<div onClick={start} className={classNames.join(' ')}>{icon}{text}</div>);
}

StartButton.propTypes = {
    application: PropTypes.string,
    cluster: PropTypes.string,
    configuration: PropTypes.string,
    onClick: PropTypes.func
};

export default StartButton;
