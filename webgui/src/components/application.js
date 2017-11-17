import React from 'react';
import { observer } from 'mobx-react';
import { Link } from 'react-router-dom';
import api from '../api';
import ClusterButton from './clusterbutton';
import StartButton from './startbutton';
import StopButton from './stopbutton';
import RestartButton from './restartbutton';
import TagLink from './taglink';
import Breadcrumbs from './breadcrumbs';
import ConfigurationButton from './configurationbutton';

class Appplication extends React.Component {
    constructor({ params: { appId }}) {
        super();
        this._appId = appId.replace('>', '/');
    }

    render() {
        const application = api.applications.find((app) => {
            return app.id === this._appId;
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
                    <ConfigurationButton key={c.id} configuration={c.name}/>
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

        const clusters = application.clusters.map((clusterId) => {
            const cluster = api.clusters.find((c) => {
                return c.id === clusterId;
            });
            if (!cluster) {
                return null;
            }

            const contents = [];

            const clusterProcesses = api.processes.filter((process) => {
                return process.clusterId === clusterId && process.clusterStatus !== 'Exit';
            });

            let running = false;
            clusterProcesses.forEach((process) => {
                if (process.applicationId === application.id) {
                    running = true;
                    contents.unshift(<StopButton key={'stop' + process.id} type="application" process={process}/>);
                    contents.unshift(<RestartButton key={'restart#' + process.id} type="application" process={process}/>);
                } else {
                    contents.push(<StopButton key={'stop' + process.id} type="application" process={process}/>);
                }
            });

            if (!running) {
                contents.unshift(<StartButton key="start" type="application" application={application} cluster={cluster}/>);
            }

            return (
                <ClusterButton key={cluster.id} cluster={cluster}>
                    {contents}
                </ClusterButton>
            );
        });


/*
        const processes = api.processes.filter((process) => {
            return process.applicationId === application.id;
        });

        const activeProcesses = processes.map((process) => {
            return <ProcessButton key={process.id} process={process}/>;
        });


        if (activeProcesses.length === 0) {
            activeProcesses.push(<span key="no-hits" className="no-hits">{application.name} is not running anywhere right now.</span>);
        }

         <div className="row">
                    <h2>Active Processes</h2>
                    {activeProcesses}
                </div>
*/

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
                    {clusters}
                </div>
            </div>
        );
    }
}

export default Appplication;
