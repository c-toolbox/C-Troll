import React from 'react';
import { observer } from 'mobx-react';
import api from '../api.js';
import moment from 'moment';

@observer
class Process extends React.Component {
    constructor({ params: { processId }}) {
        super();
        this._processId = +processId;
        console.log(this._processId);
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

        const nodeStatusHistory = process.nodeStatusHistory.map((status) => {
            return <p key={status.time}>{moment(status.time).format('YYYY MM DD HH:mm:ss')} : {status.node} {status.status}</p>;
        });


        return (
            <div>
                <div className="row">
                    <h1>{application.name}</h1>
                </div>
                <div className="row">
                    <p>{process.clusterStatus}</p>
                </div>
                <div className="row">
                    {nodeStatusHistory}
                </div>
            </div>
        );
    }
}

export default Process;
