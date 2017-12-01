import React from 'react';
import PropTypes from 'prop-types';
import './breadcrumbs.css';

const rightArrowIcon = (<svg className="right-arrow" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">
    <polygon points="8.53 64 0 55.47 23.47 32 0 8.53 8.53 0 40.53 32 8.53 64"/>
</svg>);

const wrap = (elem, key) => {
    return (<div key={key}>{elem}</div>);
};

class Breadcrumbs extends React.Component {
    render() {
        const elements = [];
        if (this.props.children) {
            for (let i = 0; i < this.props.children.length; i++) {
                if (i > 0) {
                    elements.push(wrap(rightArrowIcon, i * 2));
                }
                elements.push(wrap(this.props.children[i], i * 2 + 1));
            }
        }

        return (<div className="breadcrumbs">{elements}</div>);
    }
}

Breadcrumbs.propTypes = {
    children: PropTypes.array
};

export default Breadcrumbs;