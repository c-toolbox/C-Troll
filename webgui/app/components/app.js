import React from 'react';
import { Link } from 'react-router';
import api from '../api';

class App extends React.Component {
    componentDidMount() {
        api.initialize();
    }

    render() {
        const clustersActive = this.context.router.location.pathname.indexOf('/clusters') === 0;
        const appsActive = !clustersActive;

        const appsIcon = (<svg className="icon" xmlns="http://www.w3.org/2000/svg" viewBox="-2 -2 68 68">
            <path className="icon-path" d="M61.83,35.49L5.65,63.58A3.91,3.91,0,0,1,0,60.09V3.91A3.91,3.91,0,0,1,5.65.42L61.83,28.51A3.91,3.91,0,0,1,61.83,35.49Z"/>
        </svg>);

        const clustersIcon = (<svg className="icon" xmlns="http://www.w3.org/2000/svg" viewBox="-2 -2 68 68">
            <polygon points="54.55 19.53 64 24.99 64 35.9 54.55 41.35 45.11 35.9 45.11 24.99 54.55 19.53"/>
            <polygon points="41.44 35.9 32 41.35 22.55 35.9 22.55 24.99 32 19.53 41.44 24.99 41.44 35.9"/>
            <polygon points="18.89 35.9 9.45 41.35 0 35.9 0 24.99 9.45 19.53 18.89 24.99 18.89 35.9"/>
            <polygon points="11.28 16.36 11.28 5.45 20.72 0 30.17 5.45 30.17 16.36 20.72 21.82 11.28 16.36"/>
            <polygon points="43.28 39.06 52.72 44.52 52.72 55.43 43.28 60.88 33.83 55.43 33.83 44.52 43.28 39.06"/>
            <polygon points="30.17 55.43 20.72 60.88 11.28 55.43 11.28 44.52 20.72 39.06 30.17 44.52 30.17 55.43"/>
            <polygon points="33.83 16.36 33.83 5.45 43.28 0 52.72 5.45 52.72 16.36 43.28 21.82 33.83 16.36"/>
        </svg>);

        return (
            <div>
                <nav id="side-bar">
                    <Link className={appsActive ? 'active' : ''} to="/applications">{appsIcon}Applications</Link>
                    <Link className={clustersActive ? 'active' : ''} to="/clusters">{clustersIcon}Clusters</Link>
                </nav>
                <div id="main-container">
                    {this.props.children}
                </div>
            </div>
        );
    }
}

App.propTypes = {
    children: React.PropTypes.object.isRequired
};

App.contextTypes = {
    router: React.PropTypes.object.isRequired
};

export default App;