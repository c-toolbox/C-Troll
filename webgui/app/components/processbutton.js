import React from 'react';
import { observer } from 'mobx-react';

@observer
class ProcessButton extends React.Component {
    render() {
        const process = this.props.process;

        return (
            <div  className="button-square no-select">
                <div className="application-icon"></div>
                <div className="main">{process.name}</div>
            </div>);
    }
}

ProcessButton.propTypes = {
    process: React.PropTypes.object.isRequired
};

export default ProcessButton;
