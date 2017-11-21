import React from 'react';
import propTypes from 'prop-types';
import { Link } from 'react-router-dom';

import ApplicationsIcon from './applicationsicon';
import ClustersIcon from './clustersicon';

import './navigationsidebar.css';

const NavigationSidebar = (props) => (
        <nav id="side-bar">
            <Link className={props.selected === 'applications' ? 'active' : ''} to="/applications">
                <ApplicationsIcon/>
                Applications
            </Link>
            <Link className={props.selected === 'clusters' ? 'active' : ''} to="/clusters">
                <ClustersIcon/>
                Clusters
            </Link>
        </nav>
    );


NavigationSidebar.propTypes = {
    selected: propTypes.string
}

export default NavigationSidebar;
