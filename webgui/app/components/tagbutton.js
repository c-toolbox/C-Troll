import React from 'react';
import TagBox from './tagbox';

class TagButton extends React.Component {
    render() {
        const classNames = ['button-wide', 'dark', 'no-select'];

        if (this.props.selected) {
            classNames.push('selected');
        }

        return (<a onClick={this.props.onClick} className={classNames.join(' ')}>
            <TagBox tag={this.props.tag}/>
            {this.props.tag}
        </a>);
    }
}

TagButton.propTypes = {
    tag: React.PropTypes.string.isRequired,
    selected: React.PropTypes.bool,
    onClick: React.PropTypes.func
};

export default TagButton;
