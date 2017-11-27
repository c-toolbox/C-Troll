import React from 'react';
import TagBox from './tagbox';
import { Link } from 'react-router-dom';
import MoreButton from './morebutton';
import PropTypes from 'prop-types';

const ProcessButton = props => {
    const id = '' + props.processId;
    const applicationName = props.applicationName;
    const status = props.status;
    const clusterName = props.clusterName;
    const tags = props.tags;
    const processUrl = '/process/' + id.replace('/', '>');
    
    return (
        <div className="square button no-select">
            <Link className="click-area" to={processUrl}/>
            <div className="application-icon"/>
            <div className="main">{applicationName}</div>
            <div className="sub">{status} @ {clusterName}</div>
            {props.children}
            <MoreButton to={processUrl}/>
        </div>);
}

ProcessButton.propTypes = {
    processId: PropTypes.number.isRequired,
    applicationName: PropTypes.string.isRequired,
    clusterName: PropTypes.string.isRequired,
    status: PropTypes.string.isRequired,
    tags: PropTypes.array,
};

export default ProcessButton;
