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
        if (this.filterString.length === 0) {
            return processes;
        }

        const filterByWord = (procs, word) => {
            return procs.filter((process) => {
                const app = this.getApplication(process);
                if (!app) return false;

                const cluster = this.getCluster(process);
                if (!cluster) return false;

                const lowerClusterName = cluster.name.toLowerCase();
                const lowerApplicationName = app.name.toLowerCase();
                const lowerApplicationId = app.id.toLowerCase();

                const inApplicationName = this.nameMatchesFilter(lowerApplicationName, word);
                const inClusterName = this.nameMatchesFilter(lowerClusterName, word);
                const inApplicationId = this.nameMatchesFilter(lowerApplicationId, word);
                let inTags = false;

                app.tags.forEach((tag) => {
                    const lowerTag = tag.toLowerCase();
                    if (this.nameMatchesFilter(lowerTag, word)) {
                        inTags = true;
                        return false;
                    }
                    return true;
                });

                return inApplicationName || inApplicationId || inClusterName || inTags;
            });
        };

        const lowerFilterString = this.filterString.toLowerCase();
        const filters = lowerFilterString.split(' ');

        let filtered = processes.slice(0);
        filters.forEach((filter) => {
            filtered = filterByWord(filtered, filter);
        });

        return filtered;
    }

    nameMatchesFilter(word, filter) {
        return !!word.match(new RegExp('^' + filter));
    }
}

export default ProcessList;
