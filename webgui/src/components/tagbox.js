import React from 'react';
import ColorHash from 'color-hash';
import PropTypes from 'prop-types';

const colorHash = new ColorHash({lightness: [0.6, 0.8, 0.7], saturation: [1.0, 0.8]});

class TagBox extends React.Component {
    render() {
        return (
            <div
                className="tag"
                title={this.props.tag}
                style={{backgroundColor: colorHash.hex(this.props.tag)}}/>
        );
    }
}

TagBox.propTypes = {
    tag: PropTypes.string.isRequired
};

export default TagBox;
