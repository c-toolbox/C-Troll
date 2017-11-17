import React from 'react';
import api from '../api';
import ProcessButton from './processbutton';
import { observer } from 'mobx-react';

class FilteredProcesses extends React.Component {

    constructor({ processList, hideOnExit }) {
        super();
        this._processList = processList;
        this._hideOnExit = hideOnExit || true;
    }

    render() {
        const activeFilter = (proc) => {
            return proc.clusterStatus !== 'Exit';
        };

        const processes = this._processList.processes();
        const activeProcesses = this._processList.processes().filter(activeFilter);
        let filteredProcesses = this._processList.filteredProcesses();

        if (this._hideOnExit) {
            filteredProcesses = filteredProcesses.filter(activeFilter);
        }

        const processElements = filteredProcesses.map((proc) => <ProcessButton key={proc.id} process={proc}/>);
        if (processElements.length > 0) {
            return (
                <div>
                    <div className="row">
                        <h2>Active Processes</h2>
                    </div>
                    <div className="row button-container">
                        {processElements}
                    </div>
                </div>
            );
        }
        if (!api.initialized) {
            return null;
        }
        if (this._hideOnExit && activeProcesses.length === 0) {
            return (
                <div className="row">
                    <h2>Active Processes</h2>
                    <p>No processes are running.</p>
                </div>
            );
        } else if (processes.length === 0) {
            return (
                <div className="row">
                    <h2>Active Processes</h2>
                    <p>No processes have been started.</p>
                </div>
            );
        }
        return (
            <div className="row">
                <h2>Active Processes</h2>
                <p className="no-hits">No processes were found.</p>
            </div>
        );
    }
}

export default FilteredProcesses;
