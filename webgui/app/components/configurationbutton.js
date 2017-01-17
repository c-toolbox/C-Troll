import React from 'react';

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
    configuration: React.PropTypes.string.isRequired,
    selected: React.PropTypes.bool,
    onClick: React.PropTypes.func
};

export default ConfigurationButton;
