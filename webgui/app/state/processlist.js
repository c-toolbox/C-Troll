import { observable, computed } from 'mobx';
import api from '../api';

class ProcessList {
    @observable filterTags = [];

    @observable filterString = '';

    @computed processes = () => {
        return api.processes;
    }

    @computed filteredProcesses = () => {
        const tagFiltered = this.filterByTags(api.processes);
        const filtered = this.filterByString(tagFiltered);
        return filtered;
    };

    getApplication(process) {
        return api.applications.find((app) => {
            return app.id === process.applicationId;
        });
    }

    getCluster(process) {
        return api.clusters.find((cluster) => {
            return cluster.id === process.clusterId;
        });
    }


    filterByTags(processes) {
        if (this.filterTags.length === 0) {
            return processes;
        }
        return processes.filter((process) => {
            let found = false;
            const app = this.getApplication(process);
            if (!app) return false;

            this.filterTags.forEach((tag) => {
                if (app.tags.indexOf(tag) !== -1) {
                    found = true;
                    return false;
                }
                return true;
            });
            return found;
        });
    }

    filterByString(processes) {
        const lowerFilterString = this.filterString.toLowerCase();

        if (this.filterString.length === 0) {
            return processes;
        }
        return processes.filter((process) => {
            const app = this.getApplication(process);
            if (!app) return false;

            const cluster = this.getCluster(process);
            if (!cluster) return false;

            const lowerClusterName = cluster.name.toLowerCase();
            const lowerApplicationName = app.name.toLowerCase();
            const lowerApplicationId = app.id.toLowerCase();

            const inApplicationName = lowerApplicationName.indexOf(lowerFilterString) !== -1;
            const inClusterName = lowerClusterName.indexOf(lowerFilterString) !== -1;
            const inApplicationId = lowerApplicationId.indexOf(lowerFilterString) !== -1;
            let inTags = false;

            app.tags.forEach((tag) => {
                const lowerTag = tag.toLowerCase();
                if (lowerTag.indexOf(lowerFilterString) !== -1) {
                    inTags = true;
                    return false;
                }
                return true;
            });

            return inApplicationName || inApplicationId || inClusterName || inTags;
        });
    }
}

export default ProcessList;
