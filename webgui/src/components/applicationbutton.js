import React from 'react';
import { observer } from 'mobx-react';
import TagBox from './tagbox';
// import { Link } from 'react-router';
import StartButton from './startbutton';
import MoreButton from './morebutton';
import api from '../api';
import PropTypes from 'prop-types';

class ApplicationButton extends React.Component {
    render() {
        const application = this.props.application;

        const appUrl = '/applications/' + application.id.replace('/', '>');
        const buttons = [];

        if (application.clusters.length > 0) {
            const clusterId = application.clusters[0];
            const cluster = api.clusters.find((c) => {
                return c.id === clusterId;
            });
            if (cluster) {
                buttons.push((<StartButton key="start" type="cluster" application={application} cluster={cluster}/>));
            } else {
                buttons.push(<MoreButton key="disabled"/>);
            }
        } else {
            buttons.push(<MoreButton key="disabled"/>);
        }

        buttons.push(<MoreButton to={appUrl} key="more"/>);

        /*const open = () => {
            browserHistory.push(appUrl);
        };*/

        return (
            <div  className="square button no-select">
                <div className="click-area"/>
                <div className="tags">
                {application.tags.map((tag) => {
                    return (<TagBox key={tag} tag={tag}/>);
                })}
                </div>
                <div className="application-icon"/>
                <div className="main">{application.name}</div>
                <div className="sub"/>
                {buttons}
            </div>);
    }
}

ApplicationButton.propTypes = {
    application: PropTypes.object.isRequired
};

export default ApplicationButton;
