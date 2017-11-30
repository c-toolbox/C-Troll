import React from 'react';
import propTypes from 'prop-types';
import { Link } from 'react-router-dom';

import ApplicationsIcon from './applicationsicon';
import ClustersIcon from './clustersicon';
import ReloadConfigIcon from './reloadconfigicon';

import './navigationsidebar.css';

const NavigationSidebar = props => {
    const reloadConfigStyles = ['reload-config'];
    if (props.isReloadingConfig) {
        reloadConfigStyles.push('reloading');
    }

    return (<div id="side-bar">
        <nav>
            <Link className={props.selected === 'applications' ? 'active' : ''} to="/applications">
                <ApplicationsIcon/>
                Applications
            </Link>
            <Link className={props.selected === 'clusters' ? 'active' : ''} to="/clusters">
                <ClustersIcon/>
                Clusters
            </Link>
        </nav>
        <div className={reloadConfigStyles.join(' ')} onClick={props.onReloadConfig}>
            <ReloadConfigIcon/>
            Reload Configuration
        </div>
    </div>
)};


NavigationSidebar.propTypes = {
    selected: propTypes.string,
    isReloadingConfig: propTypes.bool.isRequired
}

export default NavigationSidebar;
