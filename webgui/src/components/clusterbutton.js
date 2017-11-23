import React from 'react';
import { Link } from 'react-router-dom';
import MoreButton from './morebutton';
import PropTypes from 'prop-types';

const icon = (<svg className="cluster-icon" xmlns="http://www.w3.org/2000/svg" viewBox="-6 -2 64 64">
    <polygon points="25.42 0 50.83 14.67 50.83 44.02 25.42 58.7 0 44.02 0 14.67 25.42 0"/>
</svg>);

const ClusterButton = props => {
    const id = props.clusterId;
    const name = props.clusterName;
    const appUrl = '/cluster/' + id.replace('/', '>');
    
    return (
        <div className="square button no-select">
            {icon}
            <Link className="click-area" to={appUrl}/>
            <div className="cluster-icon"/>
            <div className="main">{name}</div>
            <div className="sub"/>
            {props.children}
            <MoreButton to={appUrl}/>
        </div>);
}

ClusterButton.propTypes = {
    applicationId: PropTypes.string.isRequired,
    applicationName: PropTypes.string.isRequired,
    tags: PropTypes.array,
};

export default ClusterButton;



/*import React from 'react';
import MoreButton from './morebutton';
import PropTypes from 'prop-types';

class ClusterButton extends React.Component {
    render() {
        const icon = (<svg className="cluster-icon" xmlns="http://www.w3.org/2000/svg" viewBox="-6 -2 64 64">
            <polygon points="25.42 0 50.83 14.67 50.83 44.02 25.42 58.7 0 44.02 0 14.67 25.42 0"/>
        </svg>);

        const cluster = this.props.cluster;
        const clusterUrl = '/clusters/' + cluster.id.replace('/', '>');
        const classNames = ['square', 'button', 'no-select'];

        const subs = [];
        if (!cluster.connected) {
            classNames.push('error');
            subs.push(<span key="0">Disconnected</span>);
        }

        return (
            <div to={'/clusters/' + cluster.id.replace('/', '>')} className={classNames.join(' ')}>
                {icon}
                <div className="click-area"/>
                <div className="main">{cluster.name}</div>
                <div className="sub">
                    {subs}
                </div>
                {this.props.children}
                <MoreButton to={clusterUrl} key="more"/>
            </div>);
    }
}

ClusterButton.propTypes = {
    children: PropTypes.array,
    cluster: PropTypes.object.isRequired
};

export default ClusterButton;
*/