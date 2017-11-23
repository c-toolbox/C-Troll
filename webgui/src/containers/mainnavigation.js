import { connect } from 'react-redux';
import NavigationSidebar from '../components/navigationsidebar'
import { withRouter } from 'react-router-dom';

const mapStateToProps = (state, ownProps) => {
    let selected = '';
    let pathname = ownProps.location.pathname;

    if (pathname.indexOf('/application') !== -1) {
        selected = 'applications';
    } else if (pathname.indexOf('/cluster') !== -1) {
        selected = 'clusters';
    } else {
        selected = 'applications';
    }

    return {
        selected
    };
}

export default withRouter(connect(mapStateToProps)(NavigationSidebar));
