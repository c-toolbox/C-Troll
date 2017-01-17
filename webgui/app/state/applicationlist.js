import { observable, computed } from 'mobx';
import api from '../api';

class ApplicationList {
    @observable filterTags = [];

    @observable filterString = '';

    @computed tags = () => {
        const tagMap = {};
        api.applications.forEach((application) => {
            application.tags.forEach((tag) => {
                tagMap[tag] = {
                    inFilter: this.filterTags.indexOf(tag) !== -1
                };
            });
        });
        return tagMap;
    };

    @computed filteredApplications = () => {
        const tagFiltered = this.filterByTags(api.applications);
        const filtered = this.filterByString(tagFiltered);
        return filtered;
    };

    filterByTags(apps) {
        if (this.filterTags.length === 0) {
            return apps;
        }
        return apps.filter((app) => {
            let found = false;
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

    filterByString(apps) {
        const lowerFilterString = this.filterString.toLowerCase();

        if (this.filterString.length === 0) {
            return apps;
        }
        return apps.filter((app) => {
            const lowerName = app.name.toLowerCase();
            const lowerIdentifier = app.identifier.toLowerCase();

            const inName = lowerName.indexOf(lowerFilterString) !== -1;
            const inIdentifier = lowerIdentifier.indexOf(lowerFilterString) !== -1;
            let inTags = false;

            app.tags.forEach((tag) => {
                const lowerTag = tag.toLowerCase();
                if (lowerTag.indexOf(lowerFilterString) !== -1) {
                    inTags = true;
                    return false;
                }
                return true;
            });

            return inName || inIdentifier || inTags;
        });
    }
}

export default ApplicationList;
