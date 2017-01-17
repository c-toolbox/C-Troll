import React from 'react';
import { Link } from 'react-router';

class MoreButton extends React.Component {
    render() {
        const icon = (<svg className="more-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
            <circle className="more-icon-circle" cx="6.92" cy="32" r="6.92"/><circle className="more-icon-circle" cx="32.36" cy="32" r="6.92"/><circle className="more-icon-circle" cx="57.08" cy="32" r="6.92"/>
        </svg>);

        const classNames = ['more-button', 'no-select', 'sub-button'];
        return (<Link to={this.props.to} className={classNames.join(' ')}>
            {icon}
        </Link>);
    }
}

MoreButton.propTypes = {
    to: React.PropTypes.string
};

export default MoreButton;
