import React from 'react';
import PropTypes from 'prop-types';

const NodeButton = props => {
    return (
        <div to={''} className="button-wide dark no-select">{props.node.name}</div>
    );
}

NodeButton.propTypes = {
    node: PropTypes.object.isRequired
};

export default NodeButton;
