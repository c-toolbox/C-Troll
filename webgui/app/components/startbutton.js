import React from 'react';

class StartButton extends React.Component {
    render() {
        const classNames = ['start-button', 'no-select', 'sub-button'];

        const icon = (<svg className="start-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
            <path className="start-icon-path" d="M61.83,35.49L5.65,63.58A3.91,3.91,0,0,1,0,60.09V3.91A3.91,3.91,0,0,1,5.65.42L61.83,28.51A3.91,3.91,0,0,1,61.83,35.49Z"/>
        </svg>);

        const start = (evt) => {
            console.log('starting app!');
            evt.stopPropagation();
        };

        switch (this.props.type) {
            case 'application':
                return (<div onClick={start} className={classNames.join(' ')}>
                    {icon} {this.props.application.name}
                </div>);
            case 'cluster':
                return (<div onClick={start} className={classNames.join(' ')}>
                    {icon} {this.props.cluster.name}
                </div>);
            case 'both':
            default:
                return (<div onClick={start} className={classNames.join(' ')}>
                    {icon} {this.props.application.name} on {this.props.cluster.name}
                </div>);
        }
    }
}

StartButton.propTypes = {
    application: React.PropTypes.object,
    cluster: React.PropTypes.object,
    type: React.PropTypes.string
};

export default StartButton;