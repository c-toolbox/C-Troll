import React from 'react';
import { observer } from 'mobx-react';
import { Link } from 'react-router';
import api from '../api';
import ClusterButton from './clusterbutton';
import StartButton from './startbutton';
import TagLink from './taglink';
import Breadcrumbs from './breadcrumbs';
import ConfigurationButton from './configurationbutton';

@observer
class Appplication extends React.Component {
    constructor({ params: { appId }}) {
        super();
        this._appId = appId.replace('>', '/');
    }

    render() {
        const application = api.applications.find((app) => {
            return app.identifier === this._appId;
        });

        const pageName = application ? (<span>{application.name}</span>) : (<span>{this._appId}</span>);

        const breadcrumbs = (
            <Breadcrumbs>
                <Link to="/applications">Applications</Link>
                {pageName}
            </Breadcrumbs>
        );

        if (application === undefined) {
            return breadcrumbs;
        }

        const tags = [];
        if (application.tags.length > 0) {
            tags.push(<em key={'_tagged'}>Tagged:</em>);
        }

        application.tags.forEach((tag) => {
            tags.push(
                <TagLink key={tag} tag={tag}/>
            );
        });

        let configurationRow = null;


        if (application.configurations.length > 0) {
            const configurations = [];

            application.configurations.forEach((c) => {
                configurations.push(
                    <ConfigurationButton key={c.identifier} configuration={c.name}/>
                );
            });

            configurationRow = (
                <div className="row">
                        <h2>Configurations</h2>
                        {configurations}
                </div>
            );
        }

        let descriptionRow = null;
        if (application.description) {
            descriptionRow = (
                <div className="row">
                    <h2>Description</h2>
                    <p>{application.description}</p>
                </div>
            );
        }

        return (
            <div>
                {breadcrumbs}
                <div className="row">
                    <h1>{application.name}</h1>
                </div>
                {descriptionRow}
                <div className="row">
                    {tags}
                </div>
                {configurationRow}
                <div className="row">
                    <h2>Clusters</h2>
                </div>
                <div className="row">
                    {
                        application.clusters.map((clusterId) => {
                            const cluster = api.clusters.find((c) => {
                                return c.id === clusterId;
                            });
                            if (!cluster) {
                                return null;
                            }
                            return (
                                <ClusterButton key={cluster.id} cluster={cluster}>
                                    <StartButton type="application" application={application} cluster={cluster}/>
                                </ClusterButton>
                            );
                        })
                    }
                </div>
            </div>
        );
    }
}

export default Appplication;
