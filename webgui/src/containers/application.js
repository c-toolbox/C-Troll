import { connect } from 'react-redux';

import React from 'react';
import ApplicationDetails from './applicationdetails';
import Breadcrumbs from '../components/breadcrumbs';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';

const Application = props => {
    if (props.loading) {
        return null;
    } else if (props.found) {
        return <ApplicationDetails applicationId={props.applicationId}/>
    } else {
        return (
            <Breadcrumbs>
                <Link to="applications">Applications</Link>
                Not found
            </Breadcrumbs>
        )
    }
};

Application.propTypes = {
    loading: PropTypes.bool.isRequired,
    found: PropTypes.bool.isRequired,
    applicationId: PropTypes.string
};

const mapStateToProps = (state, ownProps) => {
    const applicationId = ownProps.match.params.applicationId.replace('>', '/');
    const application = state.model.applications[applicationId];

    if (application) {
        return {
            loading: false,
            found: true,
            applicationId
        }
    } else {
        return {
            loading: state.connection.connecting,
            found: false
        }
    }
};

export default connect(mapStateToProps)(Application);

