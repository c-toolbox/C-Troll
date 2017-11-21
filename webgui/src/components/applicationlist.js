import React from 'react';
import PropTypes from 'prop-types';
import ApplicationButton from '../containers/applicationbutton';

const ApplicationList = (props) => (
	<div className="row">
		{
			props.applicationIds.map((applicationId) => (
				<ApplicationButton applicationId={applicationId}
								   key={applicationId}/>
			))
		}
	</div>
);

ApplicationList.propTypes = {
	applicationIds: PropTypes.array.isRequired
};

export default ApplicationList;