import React from 'react';
import api from '../api';
import ClusterButton from './clusterbutton';
import StopButton from './stopbutton';
import RestartButton from './restartbutton';

class Clusters extends React.Component {
    render() {
        return (
            <div>
                <div className="row">
                    <h2>Clusters</h2>
                </div>
                <div className="row button-container">
                    {
                        api.clusters.map((cluster) => {
                            const contents = [];
                            const clusterProcesses = api.processes.filter((process) => {
                                return process.clusterId === cluster.id && process.clusterStatus !== 'Exit';
                            });

                            clusterProcesses.forEach((process) => {
                                contents.unshift(<StopButton key={'stop' + process.id} type="application" process={process}/>);
                                contents.unshift(<RestartButton key={'restart#' + process.id} type="application" process={process}/>);
                            });

                            return <ClusterButton key={cluster.id} cluster={cluster}>{contents}</ClusterButton>;
                        })
                    }
                </div>
            </div>
        );
    }
}


export default Clusters;
