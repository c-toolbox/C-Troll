import React from 'react';
import { Link } from 'react-router-dom';
import Breadcrumbs from './breadcrumbs';
import TagLink from './taglink';
import PropTypes from 'prop-types';

const tagLink = tag => <TagLink key={tag} tag={tag}/>;

const tagList = (props) => {
    if (!props.tags || props.tags.length === 0) {
        return null;
    }
    return (<div className="row">{props.tags.map(tag => tagLink(tag))}</div>);
}

const configurations = (props) => {
    const buttons = props.configurationButtons;
    if (!buttons || buttons.length === 0) {
        return null;
    }
    return (
        <div>
            <div className="row">
                <h2>Configurations</h2>
            </div>
            {buttons}
        </div>
    );
}

const clusters = (props) => {
    const buttons = props.clusterButtons;
    if (!buttons || buttons.length === 0) {
        return (
            <div className="row">
                <h2>No clusters</h2>
                <p>This application is not available on any cluster.</p>
            </div>
        );
    }
    return (
        <div>
            <div className="row">
                <h2>Clusters</h2>
            </div>
            {buttons}
        </div>
    );
}


const ApplicationDetails = props => {
    return (
        <div>
            <Breadcrumbs>
                <Link to="/applications">Applications</Link>
                {props.name}
            </Breadcrumbs>
            <div className="row">
                <h1>{props.name}</h1>
            </div>
            <div className="row">
                <p>{props.description}</p>
            </div>
            { tagList(props) }
            { configurations(props) }
            { clusters(props) }
        </div>
    );
};

ApplicationDetails.propTypes = {
    name: PropTypes.string,
    description: PropTypes.string,
    tags: PropTypes.array,
    configurationButtons: PropTypes.node,
    clusterButtons: PropTypes.node
};

export default ApplicationDetails;

