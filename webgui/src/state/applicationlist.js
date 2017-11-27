import api from '../api';

class ApplicationList {
    filterTags = [];

    filterString = '';

    tags = () => {
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

    filteredApplications = () => {
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

    filterByString(applications) {
        if (this.filterString.length === 0) {
            return applications;
        }

        const filterByWord = (apps, word) => {
            return apps.filter((app) => {
                const lowerName = app.name.toLowerCase();
                const lowerId = app.id.toLowerCase();

                const inName = this.nameMatchesFilter(lowerName, word);
                const inId = this.nameMatchesFilter(lowerId, word);
                let inTags = false;

                app.tags.forEach((tag) => {
                    const lowerTag = tag.toLowerCase();
                    if (this.nameMatchesFilter(lowerTag, word)) {
                        inTags = true;
                        return false;
                    }
                    return true;
                });

                return inName || inId || inTags;
            });
        };

        const lowerFilterString = this.filterString.toLowerCase();
        const filters = lowerFilterString.split(' ');

        let filtered = applications.slice(0);
        filters.forEach((filter) => {
            filtered = filterByWord(filtered, filter);
        });

        return filtered;
    }

    nameMatchesFilter(word, filter) {
        return !!word.match(new RegExp('^' + filter));
    }
}

export default ApplicationList;
