import React from 'react';
import api from '../api';
import PropTypes from 'prop-types';

class StopButton extends React.Component {
    render() {
        const classNames = ['stop-button', 'no-select', 'sub-button'];

        const icon = (<svg className="stop-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
            <rect className="stop-icon-rect" x="0" y="0" width="64" height="64"/>
        </svg>);

        // const cluster = this.props.cluster;


        const process = this.props.process;
        const applications = api.applications;
        const clusters = api.clusters;

        const application = applications.find((a) => {
            return a.id === process.applicationId;
        });
        const cluster = clusters.find((c) => {
            return c.id === process.clusterId;
        });

        let stop = () => {};
        if (api.connected && cluster.enabled && cluster.connected) {
            stop = (evt) => {
                api.stopProcess(this.props.process.id);
                evt.stopPropagation();
            };
        } else {
            classNames.push('inactive');
        }

        switch (this.props.type) {
            case 'application':
                return (<div onClick={stop} className={classNames.join(' ')}>
                    {icon} {application.name}
                </div>);
            case 'cluster':
                return (<div onClick={stop} className={classNames.join(' ')}>
                    {icon} {cluster.name}
                </div>);
            case 'none':
                return (<div onClick={stop} className={classNames.join(' ')}>
                    {icon} Stop
                </div>);
            case 'both':
            default:
                return (<div onClick={stop} className={classNames.join(' ')}>
                    {icon} {application.name} on {cluster.name}
                </div>);
        }
    }
}

StopButton.propTypes = {
    process: PropTypes.object.isRequired,
    type: PropTypes.string
};

export default StopButton;
