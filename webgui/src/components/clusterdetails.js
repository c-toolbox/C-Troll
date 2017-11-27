import React from 'react';
import { Link } from 'react-router-dom';
import Breadcrumbs from './breadcrumbs';
import TagLink from './taglink';
import PropTypes from 'prop-types';

const nodes = (props) => {
    const buttons = props.nodeButtons;
    if (!buttons || buttons.length === 0) {
        return (
            <div className="row">
                <h2>No nodes</h2>
                <p>This cluster has no nodes.</p>
            </div>
        );
    }
    return (
        <div>
            <div className="row">
                <h2>Nodes</h2>
            </div>
            {buttons}
        </div>
    );
}

const processes = (props) => {
    const buttons = props.processButtons;
    if (!buttons || buttons.length === 0) {
        return (
            <div className="row">
                <h2>No processes</h2>
                <p>There are no processes running on this cluster.</p>
            </div>
        );
    }
    return (
        <div>
            <div className="row">
                <h2>Processes</h2>
            </div>
            <div className="row">
            {buttons}
            </div>
        </div>
    );
}




const ClusterDetails = props => {
    return (
        <div>
            <Breadcrumbs>
                <Link to="/applications">Clusters</Link>
                {props.name}
            </Breadcrumbs>
            <div className="row">
                <h1>{props.name}</h1>
            </div>
            <div className="row">
                <p>{props.description}</p>
            </div>
            { processes(props) }
            { nodes(props) }
        </div>
    );
};

ClusterDetails.propTypes = {
    name: PropTypes.string,
    description: PropTypes.string,
    processButtons: PropTypes.array,
    nodeButtons: PropTypes.array
};

export default ClusterDetails;

