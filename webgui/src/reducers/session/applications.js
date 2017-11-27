import { combineReducers } from 'redux';

import {
    SetApplicationSearchString,
    AddApplicationFilterTag,
    RemoveApplicationFilterTag,
    ClearApplicationFilterTags,
    SetApplicationFilterTagsVisibility,
} from '../../actions';

function setApplicationFilterTagsVisibility(state, action) {
    return action.payload.visible
}

function setApplicationSearchString(state, action) {
    return action.payload.searchString;
}

function addApplicationFilterTag(state, action) {
    const tag = action.payload.tag;
    if (state.indexOf(tag) !== -1) {
        return state;
    }
    const newState = [...state];
    newState.push(tag);
    return newState;
}

function removeApplicationFilterTag(state, action) {
    const tag = action.payload.tag;
    const index = state.indexOf(tag);
    if (index === -1) {
        return state;
    }
    const newState = [...state];
    newState.splice(index, 1);
    return newState;
}

function clearApplicationFilterTags(state, action) {
    return [];
}

const searchString = (state = "", action) => {
    switch (action.type) {
        case SetApplicationSearchString: return setApplicationSearchString(state, action);
        default: return state;
    }
}

const filterTags = (state = [], action) => {
    switch (action.type) {
        case AddApplicationFilterTag: return addApplicationFilterTag(state, action);
        case RemoveApplicationFilterTag: return removeApplicationFilterTag(state, action);
        case ClearApplicationFilterTags: return clearApplicationFilterTags(state, action);
        default: return state;
    }
};

const filterTagsVisibility = (state = false, action) => {
    switch (action.type) {
        case SetApplicationFilterTagsVisibility: return setApplicationFilterTagsVisibility(state, action);
        default: return state;
    }
}

export default combineReducers({
    filterTags,
    searchString,
    filterTagsVisibility
});
