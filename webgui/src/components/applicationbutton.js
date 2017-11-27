import React from 'react';
import TagBox from './tagbox';
import { Link } from 'react-router-dom';
import PropTypes from 'prop-types';

const ApplicationButton = props => {
    const id = props.applicationId;
    const name = props.applicationName;
    const tags = props.tags;
    const appUrl = '/application/' + id.replace('/', '>');
    
    return (
        <div className="square button no-select">
            <Link className="click-area" to={appUrl}/>
            <div className="tags">
            {tags.map((tag) => {
                return (<TagBox key={tag} tag={tag}/>);
            })}
            </div>
            <div className="application-icon"/>
            <div className="main">{name}</div>
            {props.children}
        </div>);
}

ApplicationButton.propTypes = {
    applicationId: PropTypes.string.isRequired,
    applicationName: PropTypes.string.isRequired,
    tags: PropTypes.array,
};

export default ApplicationButton;
