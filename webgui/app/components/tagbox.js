import React from 'react';
import ColorHash from 'color-hash';

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
    tag: React.PropTypes.string.isRequired
};

export default TagBox;
