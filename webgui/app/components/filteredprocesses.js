import React from 'react';
import api from '../api';
import ProcessButton from './processbutton';
import { observer } from 'mobx-react';

@observer
class FilteredProcesses extends React.Component {

    constructor({ processList }) {
        super();
        this._processList = processList;
    }

    render() {
        const processes = this._processList.processes();
        const filteredProcesses = this._processList.filteredProcesses();

        if (filteredProcesses.length > 0) {
            return (
                <div>
                    <div className="row">
                        <h2>Active Processes</h2>
                    </div>
                    <div className="row button-container">
                        {filteredProcesses.map((proc) =>
                            <ProcessButton key={proc.id} process={proc}/>
                        )}
                    </div>
                </div>
            );
        }
        if (api.loading) {
            return (
                <div className="row">
                    <p className="no-hits">Loading processes...</p>
                </div>
            );
        }
        if (processes.length === 0) {
            return (
                <div className="row">
                    <h2>Active Processes</h2>
                    <p>No processes are running.</p>
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
