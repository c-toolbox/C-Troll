import React from 'react';
import PropTypes from 'prop-types';

const SearchField = (props) => {
	const onChange = (evt) => {
		props.onSearch(evt.target.value);
	}

	return (<div className="row">
	    <input placeholder={props.placeholder || ""}
	           className="search"
	           onChange={onChange}/>
	</div>);
};

SearchField.propTypes = {
	placeholder: PropTypes.string,
	onSearch: PropTypes.func
};

export default SearchField;