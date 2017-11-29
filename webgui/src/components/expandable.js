import React from 'react';
import PropTypes from 'prop-types';
import './expandable.css'

const upArrowIcon = (<svg className="expandable-icon down-arrow" xmlns="http://www.w3.org/2000/svg" viewBox="0 10 64 64">
    <polygon points="64 55.47 55.47 64 32 40.53 8.53 64 0 55.47 32 23.47 64 55.47"/>
</svg>);

const downArrowIcon = (<svg className="expandable-icon down-arrow" xmlns="http://www.w3.org/2000/svg" viewBox="0 -10 64 64">
    <polygon points="64 8.53 55.47 0 32 23.47 8.53 0 0 8.53 32 40.53 64 8.53"/>
</svg>);


const ExpandIcon = props => (
    props.expand ? downArrowIcon : upArrowIcon
);

const Expandable = props => {
    const onToggle = evt => {
        console.log(props);
        props.onToggle(!props.expanded);
    }

    if (props.expanded) {
        return (
            <div>
                <div onClick={onToggle} className="expandable-button">{props.header}</div>
                <div>{props.children}</div>
            </div>
        );
    } else {
        return (<div>
            <div onClick={onToggle} className="expandable-button">{props.header}</div>
        </div>);
    }
}

Expandable.propTypes = {
    header: PropTypes.node,
    expanded: PropTypes.bool,
    onToggle: PropTypes.func.isRequired
}

export { Expandable, ExpandIcon};


