import { connect } from 'react-redux';
import {
	setApplicationSearchString,
	addApplicationFilterTag,
	removeApplicationFilterTag, 
	clearApplicationFilterTags
} from '../actions';

import Applications from '../components/applications';

const getTags = state => {
	const filterTags = state.session.applications.filterTags;
    const allApplications = Object.values(state.model.applications);
	const tagMap = {};

    allApplications.forEach(application => {
        application.tags.forEach(tag => {
            tagMap[tag] = {
            	tag,
                inFilter: filterTags.indexOf(tag) !== -1
            };
        });
    });

    const tags = Object.values(tagMap).map((v) => {
    	return {
    		tag: v.tag,
    		inFilter: v.inFilter
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
	return allProcesses.map(getProcessId);
}


const mapStateToProps = (state) => {
	const applicationIds = getApplicationIds(state);
	const processIds = getProcessIds(state);
	const tags = getTags(state);

	return {
		applicationIds,
	    processIds,
	    tags
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


export default connect(mapStateToProps, mapDispatchToProps)(Applications);