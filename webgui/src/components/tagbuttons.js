import React from 'react';
import PropTypes from 'prop-types';
import TagButton from './tagbutton';

const tagButton = (tag, props) => {
    const select = () => {
        props.onAddTag(tag.name);
    }

    const deselect = () => {
        props.onRemoveTag(tag.name);
    }

    const selected = tag.selected;
    const onClick = selected ? deselect : select;

    return (
        <TagButton onClick={onClick}
                   key={tag.name}
                   name={tag.name} 
                   selected={selected}/>
    );
}

const TagButtons = (props) => {
	return (<div>
        {
            props.tags.map(tag => tagButton(tag, props))
        }
	</div>);
};

TagButtons.propTypes = {
    tags: PropTypes.array.isRequired,
    onAddTag: PropTypes.func,
    onRemoveTag: PropTypes.func,
    onClearTags: PropTypes.func
};

export default TagButtons;