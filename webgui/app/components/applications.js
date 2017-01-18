import React from 'react';
import ApplicationList from '../state/applicationlist';
import ProcessList from '../state/processlist';
import { observer } from 'mobx-react';
import TagButtons from './tagbuttons';
import FilteredApplications from './filteredApplications';
import FilteredProcesses from './filteredProcesses';


@observer
class Appplications extends React.Component {

    constructor({ params: { tag }}) {
        super();
        this._applicationList = new ApplicationList();
        this._processList = new ProcessList();

        if (tag !== undefined) {
            this._applicationList.filterTags.push(tag);
            this._processList.filterTags.push(tag);
        }
    }

    updateFilter(evt) {
        this._applicationList.filterString = evt.target.value;
        this._processList.filterString = evt.target.value;
    }

    render() {
        return (
            <div>
                <div className="row">
                    <input placeholder="Search applications..." className="search" onChange={this.updateFilter.bind(this)}/>
                </div>
                <TagButtons applicationList={this._applicationList}/>
                <FilteredApplications applicationList={this._applicationList}/>
                <FilteredProcesses processList={this._processList}/>
            </div>
        );
    }
}


export default Appplications;
