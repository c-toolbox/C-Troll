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


export const applicationDefaults = (state, applicationId) => {
    const application = state.model.applications[applicationId];
    if (!application) {
        return undefined;
    }

    const configurations = application.configurations;

    const defaults = application.defaults;
    if (!defaults) {
        return undefined;
    }

    const configurationId = defaults.configuration;
    const clusterId = defaults.cluster;

    const configuration = configurations[configurationId];

    if (!configuration) {
        return undefined;
    }

    if (configuration.clusters.indexOf(clusterId) === -1) {
        return undefined;
    }

    return {
        configurationId,
        clusterId
    }
}

export const isProcessActive = (state, processId) => {
    console.log(state)
    if (!state.model) { debugger; }
    const process = state.model.processes[processId];
    if (!process) {
        return false;
    }
    return !!{
        'Starting': true,
        'Running': true,
        'PartialExit': true
    }[process.clusterStatus];
}