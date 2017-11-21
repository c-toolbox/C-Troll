import React from 'react';
import TagBox from './tagbox';
// import { Link } from 'react-router';
//import StartButton from './startbutton';
//import MoreButton from './morebutton';
import PropTypes from 'prop-types';

const ApplicationButton = props => {
    const id = props.applicationId;
    const name = props.applicationName;
    const tags = props.tags;
    const appUrl = '/applications/' + id.replace('/', '>');
    
    /*const open = () => {
        browserHistory.push(appUrl);
    };*/

    return (
        <div  className="square button no-select">
            <div className="click-area"/>
            <div className="tags">
            {tags.map((tag) => {
                return (<TagBox key={tag} tag={tag}/>);
            })}
            </div>
            <div className="application-icon"/>
            <div className="main">{name}</div>
            <div className="sub"/>
            {props.children}
        </div>);
}

ApplicationButton.propTypes = {
    applicationId: PropTypes.string.isRequired,
    applicationName: PropTypes.string.isRequired,
    tags: PropTypes.array,
    children: PropTypes.array
};

export default ApplicationButton;
