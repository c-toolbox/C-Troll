import React from 'react';
import { observer } from 'mobx-react';
import api from '../api';

import StopButton from './stopbutton';
import RestartButton from './restartbutton';
import MoreButton from './morebutton';
import { browserHistory } from 'react-router';

@observer
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

        const open = () => {
            browserHistory.push(processUrl);
        };

        content.push(<MoreButton to={processUrl} key="more"/>);
        content.push(<div key="clusterStatus" className="sub">{process.clusterStatus}</div>);

        return (
            <div className="square button no-select">
                <div className="click-area" onClick={open}/>
                <div className="application-icon"/>
                {content}
                <RestartButton process={process} type="none"/>
                <StopButton process={process} type="none"/>
            </div>);
    }
}

ProcessButton.propTypes = {
    process: React.PropTypes.object.isRequired,
    type: React.PropTypes.string
};

export default ProcessButton;
