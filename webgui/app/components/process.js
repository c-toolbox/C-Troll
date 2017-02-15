import React from 'react';
import { observer } from 'mobx-react';
import api from '../api.js';
import moment from 'moment';
import StopButton from './stopbutton';
import RestartButton from './restartbutton';

@observer
class Process extends React.Component {
    constructor({ params: { processId }}) {
        super();
        this._processId = +processId;
    }

    render() {
        const process = api.processes.find((proc) => {
            return proc.id === this._processId;
        });

        if (!process) {
            return null;
        }

        const application = api.applications.find((app) => {
            return app.id === process.applicationId;
        });

        if (!application) {
            return null;
        }

        const cluster = api.clusters.find((c) => {
            return c.id === process.clusterId;
        });

        if (!cluster) {
            return null;
        }

        const nodeStatusHistory = process.nodeStatusHistory.map((status) => {
            return <p key={status.time}>{moment(status.time).format('YYYY MM DD HH:mm:ss')} : {status.node} {status.status}</p>;
        });


        return (
            <div>
                <div className="row">
                    <h1>{application.name}</h1>
                    <h2>Cluster: {cluster.name}</h2>
                </div>
                <div className="row">
                <div className="square">
                    <StopButton key={'stop' + process.id} type="none" process={process}/>
                    <RestartButton key={'restart#' + process.id} type="none" process={process}/>
                </div>
                </div>
                <div className="row">
                    <h2>Current Status</h2>
                    <p>{process.clusterStatus}</p>
                </div>
                <div className="row">
                    <h2>Log</h2>
                    {nodeStatusHistory}
                </div>
            </div>
        );
    }
}

export default Process;
