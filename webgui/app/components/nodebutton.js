import React from 'react';
import { observer } from 'mobx-react';

@observer
class NodeButton extends React.Component {
    render() {
        const node = this.props.node;

        return (
            <div to={''} className="button-wide dark no-select">{node.name}</div>);
    }
}

NodeButton.propTypes = {
    node: React.PropTypes.object.isRequired
};

export default NodeButton;
