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
    console.log(state, applicationId);
    const application = state.model.applications[applicationId];
    const configurations = application.configurations;


    // Todo: add default configuration to application config.
    // For now, just pick the first key (baiscally random)
    const firstConfig = Object.values(application.configurations)[0];
    const firstCluster = Object.values(firstConfig.clusters)[0];

    return firstCluster;
}

export const defaultApplicationConfiguration = (state, applicationId) => {
    const application = state.model.applications[applicationId];
    const configurations = application.configurations;

    // Todo: add default configuration to application config.
    // For now, just pick the first key (baiscally random)
    return Object.keys(application.configurations)[0];
}

export const isProcessActive = (process) => {
    return !!{
        'Starting': true,
        'Running': true,
        'PartialExit': true
    }[process.clusterStatus];
}