import React from 'react';
import PropTypes from 'prop-types';

const ConfigurationButton = props => {
    const classNames = ['button-wide', 'dark', 'no-select'];

    if (props.selected) {
        classNames.push('selected');
    }

    const onSelect = (evt) => {
        props.onSelect(props.configurationId);
    }

    return (<a onClick={onSelect} className={classNames.join(' ')}>
        {props.configurationName}
    </a>);
}

ConfigurationButton.propTypes = {
    configurationId: PropTypes.string.isRequired,
    configurationName: PropTypes.string.isRequired,
    selected: PropTypes.bool,
    onSelect: PropTypes.func
};

export default ConfigurationButton;
