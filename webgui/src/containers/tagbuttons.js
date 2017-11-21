import React from 'react';
import TagButton from './tagbutton';
import TagLink from './taglink';
import api from '../api';

class TagButtons extends React.Component {
    constructor({applicationList}) {
        super();
        this._applicationList = applicationList;
    }

    toggleTag(tag) {
        const index = this._applicationList.filterTags.indexOf(tag);
        if (index === -1) {
            this._applicationList.filterTags.push(tag);
        } else {
            this._applicationList.filterTags.splice(index, 1);
        }
    }

    render() {
        if (!api.initialized) {
            return null;
        }

        const filterTags = this._applicationList.filterTags;

        let ClearTags = () => null;
        if (filterTags.length > 0 ) {
            ClearTags = () => (<a onClick={() => {filterTags.splice(0); }} className={'button-wide dark'}>Clear tags</a>);
        }

        if (this._applicationList.filterString.length === 0) {
            const tags = this._applicationList.tags();
            return (
                <div>
                    <div className="row">
                        <h2>Tags</h2>
                    </div>
                    <div className="row button-container">
                        {
                            Object.keys(tags).map((tag) =>
                            <TagButton
                                onClick={() => { this.toggleTag(tag); }}
                                key={tag}
                                tag={tag}
                                selected={this._applicationList.filterTags.length === 0 || tags[tag].inFilter}/>
                        )}
                        <ClearTags/>
                    </div>
                </div>
            );
        }

        if (filterTags.length > 0) {
            return (
                <div className="row">
                    Only displaying results from: {' '}
                    {filterTags.map((tag) => (<TagLink key={tag} tag={tag}/>))}
                    <ClearTags/>
                </div>
            );
        }
        return null;
    }
}

export default TagButtons;
