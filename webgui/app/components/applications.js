import React from 'react';
import ApplicationList from '../state/applicationlist';
import { observer } from 'mobx-react';
import TagButtons from './tagbuttons';
import FilteredApplications from './FilteredApplications';

@observer
class Appplications extends React.Component {

    constructor({ params: { tag }}) {
        super();
        this._applicationList = new ApplicationList();
        if (tag !== undefined) {
            this._applicationList.filterTags.push(tag);
        }
    }

    updateFilter(evt) {
        this._applicationList.filterString = evt.target.value;
    }

    render() {
        return (
            <div>
                <div className="row">
                    <input placeholder="Search applications..." className="search" onChange={this.updateFilter.bind(this)}/>
                </div>
                <TagButtons applicationList={this._applicationList}/>
                <FilteredApplications applicationList={this._applicationList}/>
            </div>
        );
    }
}


export default Appplications;
