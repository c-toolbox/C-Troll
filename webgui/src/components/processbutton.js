import React from 'react';
import TagBox from './tagbox';
import { Link } from 'react-router-dom';
import MoreButton from './morebutton';
import PropTypes from 'prop-types';

const ProcessButton = props => {
    const id = '' + props.processId;
    const name = props.applicationName;
    const tags = props.tags;
    const processUrl = '/process/' + id.replace('/', '>');
    
    return (
        <div className="square button no-select">
            <Link className="click-area" to={processUrl}/>
            <div className="application-icon"/>
            <div className="main">{name}</div>
            <div className="sub"/>
            {props.children}
            <MoreButton to={processUrl}/>
        </div>);
}

ProcessButton.propTypes = {
    processId: PropTypes.number.isRequired,
    applicationName: PropTypes.string.isRequired,
    tags: PropTypes.array,
};

export default ProcessButton;


/*
import React from 'react';
import api from '../api';

import StopButton from './stopbutton';
import RestartButton from './restartbutton';
import MoreButton from './morebutton';

import PropTypes from 'prop-types';
//import { browserHistory } from 'react-router';

class ProcessButton extends React.Component {
    render() {
        const type = this.props.type;
        const process = this.props.process;

        const applications = api.applications;
        const clusters = api.clusters;

        const application = applications.find((a) => {
            return a.id === process.applicationId;
        }) || {name: 'Application'};
        const cluster = clusters.find((c) => {
            return c.id === process.clusterId;
        }) || {name: 'Cluster'};

        const content = [];
        switch (type) {
            case 'none':
                break;
            case 'application':
                content.push(<div key="application" className="main">{application.name}</div>);
                break;
            case 'cluster':
                content.push(<div key="cluster" className="main">{cluster.name}</div>);
                break;
            case 'both':
            default:
                content.push(<div key="application" className="main">{application.name}</div>);
                content.push(<div key="cluster" className="sub">{cluster.name}</div>);
                break;
        }

        const processUrl = '/processes/' + process.id;

        content.push(<MoreButton to={processUrl} key="more"/>);
        content.push(<div key="clusterStatus" className="sub">{process.clusterStatus}</div>);

        return (
            <div className="square button no-select">
                <div className="click-area"/>
                <div className="application-icon"/>
                {content}
                <RestartButton process={process} type="none"/>
                <StopButton process={process} type="none"/>
            </div>);
    }
}

ProcessButton.propTypes = {
    process: PropTypes.object.isRequired,
    type: PropTypes.string
};

export default ProcessButton;
*/