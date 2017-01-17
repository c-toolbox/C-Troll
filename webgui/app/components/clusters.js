import React from 'react';
import api from '../api';
import { observer } from 'mobx-react';
import ClusterButton from './clusterbutton';

@observer
class Clusters extends React.Component {
    render() {
        return (
            <div>
                <div className="row">
                    <h2>Clusters</h2>
                </div>
                <div className="row button-container">
                    {api.clusters.map((cluster) =>
                        <ClusterButton key={cluster.id} cluster={cluster}/>
                    )}
                </div>
            </div>
        );
    }
}


export default Clusters;
