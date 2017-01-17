import React from 'react';
import TagBox from './tagbox';
import { Link } from 'react-router';

class TagLink extends React.Component {
    render() {
        const classNames = ['tag-link', 'no-select'];

        return (<Link to={'/tags/' + this.props.tag} className={classNames.join(' ')}>
            <TagBox tag={this.props.tag}/>
            {this.props.tag}
        </Link>);
    }
}

TagLink.propTypes = {
    tag: React.PropTypes.string.isRequired
};

export default TagLink;
