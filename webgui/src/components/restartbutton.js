import React from 'react';
import api from '../api';
import PropTypes from 'prop-types';

class RestartButton extends React.Component {
    render() {
        const classNames = ['restart-button', 'no-select', 'sub-button'];

        const icon = (<svg className="restart-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 42 47">
            <path className="restart-icon-path" d="M31.28,18.16A16.33,16.33,0,1,0,43.2,23.32" transform="translate(-10.22 -9.14)"/>
            <path className="restart-icon-arrow" d="M42.72,18.16c-5.38-2-12.06-5.4-16.2-9l3.26,9-3.26,9C30.66,23.56,37.34,20.15,42.72,18.16Z" transform="translate(-10.22 -9.14)"/>
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
                api.restartProcess(this.props.process.id);
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
                    {icon} Restart
                </div>);
            case 'both':
            default:
                return (<div onClick={stop} className={classNames.join(' ')}>
                    {icon} {application.name} on {cluster.name}
                </div>);
        }
    }
}

RestartButton.propTypes = {
    process: PropTypes.object.isRequired,
    type: PropTypes.string
};

export default RestartButton;
