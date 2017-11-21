import React from 'react';
import PropTypes from 'prop-types';

import SearchField from './searchfield';
import TagButtons from './tagbuttons';
import ApplicationList from './applicationlist';
import ProcessList from './processlist';

const Applications = (props) => (
    <div>
        <SearchField placeholder="Search applications..." onSearch={props.onSearch}/>
        <TagButtons tags={props.tags}
                    onAddTag={props.onAddTag}
                    onRemoveTag={props.onRemoveTag}
                    onClearTags={props.onClearTags} />

        <div className="row"><h2>Applications</h2></div>
        <ApplicationList applicationIds={props.applicationIds}/>
        <div className="row"><h2>Processes</h2></div>
        <ProcessList processIds={props.processIds}/>
    </div>
);

Applications.propTypes = {
    applicationIds: PropTypes.array.isRequired,
    processIds: PropTypes.array.isRequired,
    tags: PropTypes.array.isRequired,
    onSearch: PropTypes.func.isRequired,
    onAddTag: PropTypes.func.isRequired,
    onRemoveTag: PropTypes.func.isRequired,
    onClearTags: PropTypes.func.isRequired
};

export default Applications;
