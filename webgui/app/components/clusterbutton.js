import React from 'react';
import { observer } from 'mobx-react';
import { browserHistory } from 'react-router';
import MoreButton from './morebutton';


@observer
class ClusterButton extends React.Component {
    render() {
        const icon = (<svg className="cluster-icon" xmlns="http://www.w3.org/2000/svg" viewBox="-6 -2 64 64">
            <polygon points="25.42 0 50.83 14.67 50.83 44.02 25.42 58.7 0 44.02 0 14.67 25.42 0"/>
        </svg>);

        const cluster = this.props.cluster;
        const clusterUrl = '/clusters/' + cluster.id.replace('/', '>');

        const open = () => {
            browserHistory.push(clusterUrl);
        };

        return (
            <div to={'/clusters/' + cluster.id.replace('/', '>')} className="button-square dark no-select">
                {icon}
                <div className="click-area" onClick={open}></div>
                <div className="main">{cluster.name}</div>
                <div className="sub">Running OpenSpace Develop</div>
                {this.props.children}
                <MoreButton to={clusterUrl} key="more"/>
            </div>);
    }
}

ClusterButton.propTypes = {
    children: React.PropTypes.object,
    cluster: React.PropTypes.object.isRequired
};

export default ClusterButton;
