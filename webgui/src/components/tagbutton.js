import React from 'react';
import TagBox from './tagbox';
import PropTypes from 'prop-types';

const TagButton = props => {
    const classNames = ['button-wide', 'dark', 'no-select'];

    if (props.selected) {
        classNames.push('selected');
    }

    return (
        <a onClick={props.onClick} className={classNames.join(' ')}>
            <TagBox tag={props.name}/>
            {props.name}
        </a>
    );
}

TagButton.propTypes = {
    name: PropTypes.string.isRequired,
    selected: PropTypes.bool,
    onClick: PropTypes.func.isRequired
};

export default TagButton;
