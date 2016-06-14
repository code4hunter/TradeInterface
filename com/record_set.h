#ifndef _record_set_h_
#define _record_set_h_

#include <map>
#include <string>
#include <vector>

class record_set
{
private:
	typedef std::map<std::string,size_t> FIELDNAME;
	FIELDNAME _fieldNames;
	std::vector<std::string> _fields;
	std::vector<std::string> _values;

	size_t _row;
	size_t _col;

    size_t _pos;
protected:

public:
	record_set(const std::string &text="")
	{
        _pos = 0;
		set_text(text);
	}

	record_set(const size_t ncol,const size_t nrow)
	{
		resize(ncol,nrow);
    }

    void resize(const size_t ncol,const size_t nrow)
    {
        _row = nrow;
		_col = ncol;

        _values.resize(nrow*ncol);
    }

	void clear(void){
		_fieldNames.clear();
		_fields.clear();
		_values.clear();
	}

	size_t get_column_size(void)
	{
		return _col;
	}

	size_t get_row_size(void)
	{
    	return _row;
	}

	size_t add_field_name(const std::string& name)
	{
		FIELDNAME::iterator iter =_fieldNames.find(name);
		if( iter == _fieldNames.end() )
		{
            size_t idx = _fieldNames.size();
            _fieldNames.insert(std::pair<std::string,size_t>(name,idx));
			_fields.push_back(name);
            return idx ;
		}
		else
			throw("Field name already exists!");
	}

    size_t get_field_id(const std::string& name)
    {
        FIELDNAME::const_iterator iter =_fieldNames.find(name);
		if( iter != _fieldNames.end() )
		{
            return iter->second;
        }
        else
			throw(("Can not find field:"+name).c_str());
    }

	void set_field_value(const std::string& name,const std::string& value,const size_t row=1)
	{
		FIELDNAME::const_iterator iter =_fieldNames.find(name);
		if( iter != _fieldNames.end() )
		{
			size_t index = get_column_size()*(row-1)+iter->second;
			if(_values.size()<index+1)
				throw("Index out of space!");
            else
                _values[index] = value;
        }
        else
			throw(("Can not find field:"+name).c_str());
	}

	std::string get_field_value(const std::string& name,const size_t row=1)
	{
    	FIELDNAME::const_iterator iter =_fieldNames.find(name);
		if( iter != _fieldNames.end() )
		{
			size_t index = get_column_size()*(row-1)+iter->second;
			if(_values.size()<index+1)
			{
				throw("Index out of space!");
			}
			else
				return _values[index];
		}
		else
			throw(("Can not find field:"+name).c_str());
	}

    void set_field_value(const size_t fieldId, const std::string& value,const size_t row=1)
	{
        size_t index = get_column_size()*(row-1)+fieldId;
        if(_values.size()<index+1)
            throw("Index out of space!");
        else
            _values[index] = value;
	}

	std::string get_field_value(const size_t fieldId, const size_t row=1)
	{
        size_t index = get_column_size()*(row-1)+fieldId;
        if(_values.size()<index+1)
        {
            throw("Index out of space!");
        }
        else
            return _values[index];
	}

	std::string get_text(void);
	size_t set_text(const std::string &text,size_t pos=0);
};

#endif
