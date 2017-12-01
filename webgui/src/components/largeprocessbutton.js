import React from 'react';
import { Link } from 'react-router-dom';
import PropTypes from 'prop-types';

const ProcessButton = props => {
    const id = '' + props.processId;
    const applicationName = props.applicationName;
    const status = props.status;
    const clusterName = props.clusterName;
    const processUrl = '/process/' + id.replace('/', '>');
    
    return (
        <div className="square button no-select">
            <Link className="click-area" to={processUrl}/>
            <div className="application-icon"/>
            <div className="main">{applicationName}</div>
            <div className="sub">{status} @ {clusterName}</div>
            {props.children}
        </div>);
}

ProcessButton.propTypes = {
    processId: PropTypes.number.isRequired,
    applicationName: PropTypes.string.isRequired,
    clusterName: PropTypes.string.isRequired,
    status: PropTypes.string.isRequired
};

export default ProcessButton;
