import React from 'react';
import { Link } from 'react-router-dom';
import PropTypes from 'prop-types';

class MoreButton extends React.Component {
    render() {
        const icon = (<svg className="more-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
            <circle className="more-icon-circle" cx="32" cy="6.9" r="6.9"/>
            <circle className="more-icon-circle" cx="32" cy="32" r="6.9"/>
            <circle className="more-icon-circle" cx="32" cy="57.1" r="6.9"/>
        </svg>);

        const classNames = ['more-button', 'no-select'];
        return (<Link to={this.props.to} className={classNames.join(' ')}>
            {icon}
        </Link>);
    }
}

MoreButton.propTypes = {
    to: PropTypes.string
};

export default MoreButton;
