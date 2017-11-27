import { connect } from 'react-redux';

import React from 'react';
import ProcessDetails from './processdetails';
import Breadcrumbs from '../components/breadcrumbs';
import PropTypes from 'prop-types';
import { Link } from 'react-router-dom';

const Process = props => {
    if (props.loading) {
        return null;
    } else if (props.found) {
        return <ProcessDetails processId={props.processId}/>
    } else {
        return (
            <Breadcrumbs>
                <Link to="applications">Processes</Link>
                Not found
            </Breadcrumbs>
        )
    }
};

Process.propTypes = {
    loading: PropTypes.bool.isRequired,
    found: PropTypes.bool.isRequired,
    applicationId: PropTypes.string
};

const mapStateToProps = (state, ownProps) => {
    const processId = ownProps.match.params.processId;
    const process = state.model.processes[processId];

    if (!process) {
        return {
            loading: state.connection.connecting,
            found: false
        }
    };

    return {
        loading: false,
        found: true,
        processId,
        nodeStatusHistory: process.nodeStatusHistory,
        logMessageHistory: process.logMessages
    }
};

export default connect(mapStateToProps)(Process);
