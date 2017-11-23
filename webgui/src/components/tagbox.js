import React from 'react';
import ColorHash from 'color-hash';
import PropTypes from 'prop-types';

const colorHash = new ColorHash(
	{
		lightness: [0.6, 0.8, 0.7],
		saturation: [1.0, 0.8]
	}
);

const TagBox = props => {
	const style = {
		backgroundColor: colorHash.hex(props.tag)
	};
    return (
        <div className="tag"
             title={props.tag}
             style={style}/>
    );
}

TagBox.propTypes = {
    tag: PropTypes.string.isRequired
};

export default TagBox;
