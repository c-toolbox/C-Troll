import React from 'react';
import api from '../api';
import ApplicationButton from './applicationbutton';
import { observer } from 'mobx-react';

@observer
class FilteredApplications extends React.Component {

    constructor({ applicationList }) {
        super();
        this._applicationList = applicationList;
    }

    render() {
        const filteredApplications = this._applicationList.filteredApplications();

        if (filteredApplications.length > 0) {
            return (
                <div>
                    <div className="row">
                        <h2>Applications</h2>
                    </div>
                    <div className="row button-container">
                        {filteredApplications.map((app) =>
                            <ApplicationButton key={app.identifier} application={app}/>
                        )}
                    </div>
                </div>
            );
        }
        if (api.loading) {
            return (
                <div className="row">
                    <p className="no-hits">Loading applications...</p>
                </div>
            );
        }
        return (
            <div className="row">
                <p className="no-hits">No applications were found.</p>
            </div>
        );
    }
}

export default FilteredApplications;
