import React from 'react';
import { connect } from 'react-redux';
import {
    setApplicationSearchString,
    addApplicationFilterTag,
    removeApplicationFilterTag, 
    clearApplicationFilterTags
} from '../actions';

import SearchField from '../components/searchfield';
import TagButtons from '../components/tagbuttons';
import List from '../components/list';

import ApplicationButton from './applicationbutton';
import ProcessButton from './processbutton';
import StartButton from './startbutton';
import StopButton from './stopbutton';
import RestartButton from './restartbutton';
import ApplicationProcessButtons from './applicationprocessbuttons';
import { defaultApplicationCluster,
         defaultApplicationConfiguration,
         isProcessActive } from '../query';

const getTags = state => {
    const filterTags = state.session.applications.filterTags;
    const allApplications = Object.values(state.model.applications);
    const tagMap = {};

    allApplications.forEach(application => {
        application.tags.forEach(name => {
            tagMap[name] = {
                name,
                selected: filterTags.indexOf(name) !== -1
            };
        });
    });

    const tags = Object.values(tagMap).map((v) => {
        return {
            name: v.name,
            selected: v.selected
        }
    })

    return tags;
};

const nameMatchesFilter = (word, filter) => {
    return !!word.match(new RegExp('^' + filter));
}

const getApplicationId = application => {
    return application.id;
}

const getProcessId = process => {
    return process.id;
}

const applicationTagFilter = filterTags => application => {
    if (filterTags.length === 0) {
        return true;
    }
    let found = false;
    filterTags.forEach((filterTag) => {
        if (application.tags.indexOf(filterTag) !== -1) {
            found = true;
            return false;
        }
        return true;
    });
    return found;
}

const filterApplicationByWord = (application, word) => {
    const lowerName = application.name.toLowerCase();
    const lowerId = application.id.toLowerCase();

    const inName = nameMatchesFilter(lowerName, word);
    const inId = nameMatchesFilter(lowerId, word);
    let inTags = false;

    application.tags.forEach((tag) => {
        const lowerTag = tag.toLowerCase();
        if (nameMatchesFilter(lowerTag, word)) {
            inTags = true;
            return false;
        }
        return true;
    });

    return inName || inId || inTags;
};

const applicationStringFilter = filterString => application => {
    const lowerFilterString = filterString.toLowerCase();
    const filters = lowerFilterString.split(' ');

    if (filters.length === 0) {
        return true;
    }

    let found = false;
    filters.forEach((filter) => {
        found = filterApplicationByWord(application, filter);
        if (!found) {
            return false;
        }
    });
    return found;
}

function getApplicationIds(state) {
    const tags = state.session.applications.filterTags;
    const searchString = state.session.applications.searchString;

    const allApplications = Object.values(state.model.applications);
    return allApplications
        .filter(applicationStringFilter(searchString))
        .filter(applicationTagFilter(tags))
        .map(getApplicationId);
}

function getProcessIds(state) {
    const allProcesses = Object.values(state.model.processes);
    return allProcesses.filter(process => {
        return isProcessActive(process);
    }).map(getProcessId);
}


const mapStateToProps = (state) => {
    const applicationIds = getApplicationIds(state);
    const processIds = getProcessIds(state);
    const tags = getTags(state);

    const defaultApplicationConfigurations = {};

    applicationIds.forEach((applicationId) => {
        const clusterId = defaultApplicationCluster(state, applicationId);
        const configurationId = defaultApplicationConfiguration(state, applicationId);

        if (clusterId !== undefined && configurationId !== undefined) {
            defaultApplicationConfigurations[applicationId] = {
                clusterId,
                configurationId
            }
        }
    });

    const initialSearchText = state.session.applications.searchString;

    return {
        applicationIds,
        processIds,
        tags,
        defaultApplicationConfigurations,
        initialSearchText
    }
}

const mapDispatchToProps = dispatch => {
    const onSearch = searchString => {
        dispatch(setApplicationSearchString(searchString));
    };

    const onAddTag = tag => {
        dispatch(addApplicationFilterTag(tag))
    };

    const onRemoveTag = tag => {
        dispatch(removeApplicationFilterTag(tag))
    };

    const onClearTags = tag => {
        dispatch(clearApplicationFilterTags(tag))
    };

    return {
        onSearch,
        onAddTag,
        onRemoveTag,
        onClearTags
    }
}

const Applications = (props) => {
    return (
        <div>
            <SearchField initialText={props.initialSearchText} placeholder="Search applications..." onSearch={props.onSearch}/>
            <TagButtons tags={props.tags}
                        onAddTag={props.onAddTag}
                        onRemoveTag={props.onRemoveTag}
                        onClearTags={props.onClearTags} />

            <div className="row"><h2>Applications</h2></div>
            <List>
                {
                    props.applicationIds.map((applicationId) => {
                        const conf = props.defaultApplicationConfigurations[applicationId];

                        const startButton = conf ? (
                            <StartButton applicationId={applicationId}
                                             clusterId={conf.clusterId}
                                             configurationId={conf.configurationId}
                                             cluster/>
                                             ) : null;
                        return (
                            <ApplicationButton applicationId={applicationId}
                                               key={applicationId}>
                                {startButton}
                                <ApplicationProcessButtons applicationId={applicationId}/>
                            </ApplicationButton>
                        );
                    })
                }
            </List>


            <div className="row"><h2>Processes</h2></div>
            <List>
                {
                    props.processIds.map((processId) => {
                        return (
                            <ProcessButton processId={processId}
                                               key={processId}>

                                <StopButton processId={processId} />
                                <RestartButton processId={processId} />

                            </ProcessButton>
                        );
                    })
                }

            </List>
        </div>
    )
};


export default connect(mapStateToProps, mapDispatchToProps)(Applications);