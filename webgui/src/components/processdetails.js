import React from 'react';
import { Link } from 'react-router-dom';
import Breadcrumbs from './breadcrumbs';
import TagLink from './taglink';
import PropTypes from 'prop-types';
import moment from 'moment';

const ProcessDetails = props => {

    const logMessages = props.logMessages || [];

    const logMessageHistory = logMessages.map((logMessage) => (
        <div key={logMessage.id}>
            {moment(logMessage.time).format('YYYY MM DD HH:mm:ss')}
            : {logMessage.nodeId}
            <pre>{logMessage.message}</pre>
        </div>
    ));

    const nodeStatus = props.nodeStatusHistory || [];
    const nodeStatusHistory = nodeStatus.map((status) => (
        <p key={status.id}>
            {moment(status.time).format('YYYY MM DD HH:mm:ss')}
            : {status.nodeId} {status.status}
        </p>
    ));

    return (
        <div>
            <Breadcrumbs>
                <Link to="/applications">Applications</Link>
                <span>{props.applicationName} on {props.clusterName}</span>
            </Breadcrumbs>
            <div className="row">
                <h1>{props.applicationName} on {props.clusterName}</h1>
            </div>
            <div className="row">
                <p>{props.clusterStatus}</p>
            </div>
            <div className="row">
                <h2>Log messages</h2>
                {logMessageHistory}
            </div>
            <div className="row">
                <h2>Node status history</h2>
                {nodeStatusHistory}
            </div>
        </div>
    );
};

ProcessDetails.propTypes = {
    applicationName: PropTypes.string.isRequired,
    clusterName: PropTypes.string.isRequired,
};

export default ProcessDetails;

