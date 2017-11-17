import React from 'react';
import PropTypes from 'prop-types';

class ConfigurationButton extends React.Component {
    render() {
        const classNames = ['button-wide', 'dark', 'no-select'];

        if (this.props.selected) {
            classNames.push('selected');
        }

        return (<a onClick={this.props.onClick} className={classNames.join(' ')}>
            {this.props.configuration}
        </a>);
    }
}

ConfigurationButton.propTypes = {
    configuration: PropTypes.string.isRequired,
    selected: PropTypes.bool,
    onClick: PropTypes.func
};

export default ConfigurationButton;
