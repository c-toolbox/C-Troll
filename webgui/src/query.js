export const applicationNameByProcessId = (state, processId) => {
    const process = state.model.processes[processId];
    const applications = state.model.applications;

    const application = applications.find(a => {
        return a.id = process.applicationId;
    });

    return application ? application.name : 'Unknown application';
}

export const clusterNameByProcessId = (state, processId) => {
    const process = state.model.processes[processId];
    const clusters = state.model.clusters;

    const cluster = clusters.find(c => {
        return c.id = process.clusterId;
    });

    return cluster ? cluster.name : 'Unknown cluster';
}
