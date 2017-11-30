/**
 * Action types
 */

// User actions
  // Connection:
export const ServerConnect = 'ServerConnect';
export const StartProcess = 'StartProcess';
export const StopProcess = 'StopProcess';
export const ReloadConfig = 'ReloadConfig';

  // Interact with model
export const RestartProcess = 'RestartProcess';

  // Session
export const SetApplicationSearchString = "SetApplicationSearchString";
export const AddApplicationFilterTag = "AddApplicationFilterTag";
export const RemoveApplicationFilterTag = "RemoveApplicationFilterTag";
export const ClearApplicationFilterTags = "ClearApplicationFilterTags";
export const SetApplicationFilterTagsVisibility = "SetApplicationFilterTagsVisibility";

// Server actions
export const ServerConnected = "ServerConnected";
export const ServerDisconnected = "ServerDisconnected";
export const InitializeGui = 'InitializeGui';
export const SetProcessLogMessageHistory = 'SetProcessLogMessageHistory';
export const AddProcessLogMessage = 'AddProcessLogMessage';
export const SetProcessStatus = 'SetProcessStatus';

/**
 * Action creators
 */

 // User actions
export function serverConnect(stateData) {
    return {
        type: ServerConnect
    }
}

export function startProcess(applicationId, configurationId, clusterId) {
    return {
        type: StartProcess,
        payload: {
            applicationId,
            configurationId,
            clusterId
        }
    }
}

export function restartProcess(processId) {
    return {
        type: RestartProcess,
        payload: {
            processId
        }
    }
}

export function stopProcess(processId) {
    return {
        type: StopProcess,
        payload: {
            processId
        } 
    }
}

export function reloadConfig() {
    return {
        type: ReloadConfig,
        payload: {} 
    }
}

// Server actions
export function initializeGui(data) {
    return {
        type: InitializeGui,
        payload: {
            data
        }
    }
}

export function serverConnected() {
    return {
        type: ServerConnected
    };
}

export function serverDisconnected() {
    return {
        type: ServerDisconnected
    };
}

export function setProcessStatus(data) {
    return {
        type: SetProcessStatus,
        payload: {
            data
        }
    }
}

export function addProcessLogMessage(data) {
    return {
        type: AddProcessLogMessage,
        payload: {
            data
        }
    }
}

export function setProcessLogMessageHistory(data) {
    return {
        type: SetProcessLogMessageHistory,
        payload: {
            data
        }
    }
}

  // Session
export function setApplicationSearchString(searchString) {
    return {
        type: SetApplicationSearchString,
        payload: {
            searchString
        }
    }
};

export function addApplicationFilterTag(tag) {
    return {
        type: AddApplicationFilterTag,
        payload: {
            tag
        }
    }
};

export function removeApplicationFilterTag(tag) {
    return {
        type: RemoveApplicationFilterTag,
        payload: {
            tag
        }
    }
};

export function clearApplicationFilterTags() {
    return {
        type: SetApplicationSearchString,
        payload: {}
    }
};

export function setApplicationFilterTagsVisibility(visible) {
    return {
        type: SetApplicationFilterTagsVisibility,
        payload: {
            visible
        }
    };
}