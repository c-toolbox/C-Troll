import React from 'react';
import PropTypes from 'prop-types';

const ConfigurationButton = props => {
    const classNames = ['button-wide', 'dark', 'no-select'];

    if (props.selected) {
        classNames.push('selected');
    }

    return (<a onClick={props.onClick} className={classNames.join(' ')}>
        {props.configuration}
    </a>);
}

ConfigurationButton.propTypes = {
    configuration: PropTypes.string.isRequired,
    selected: PropTypes.bool,
    onClick: PropTypes.func
};

export default ConfigurationButton;
