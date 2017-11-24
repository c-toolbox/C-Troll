export const applicationNameByProcessId = (state, processId) => {
    const process = state.model.processes[processId];
    const applications = state.model.applications;

    const application = Object.values(applications).find(a => {
        return a.id === process.applicationId;
    });

    return application ? application.name : 'Unknown application';
}

export const clusterNameByProcessId = (state, processId) => {
    const process = state.model.processes[processId];
    const clusters = state.model.clusters;

    const cluster = Object.values(clusters).find(c => {
        return c.id === process.clusterId;
    });

    return cluster ? cluster.name : 'Unknown cluster';
}


export const defaultApplicationCluster = (state, applicationId) => {
    const application = state.model.applications[applicationId];
    return application.clusters[0];
}

export const defaultApplicationConfiguration = (state, applicationId) => {
    const application = state.model.applications[applicationId];
    const configurations = application.configurations;
    return configurations[0] && configurations[0].id;
}

export const isProcessActive = (process) => {
    return !!{
        'Starting': true,
        'Running': true,
        'PartialExit': true
    }[process.clusterStatus];
}