import React from 'react';
import api from '../api';
import ApplicationButton from './applicationbutton';

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
                            <ApplicationButton key={app.id} application={app}/>
                        )}
                    </div>
                </div>
            );
        }
        if (!api.initialized) {
            return null;
        }
        return (
            <div className="row">
                <h2>Applications</h2>
                <p className="no-hits">No applications were found.</p>
            </div>
        );
    }
}

export default FilteredApplications;
