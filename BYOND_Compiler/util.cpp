#include "util.hpp"
#if 0
void string_builder::_resize() {
	std::vector<char> new_table;
	new_table.reserve(_current_pos + block_size);
	new_table.assign(_table.begin(), _table.end());
	lookup_t new_lookup(_lookup.bucket_count());

	std::transform(_lookup.begin(), _lookup.end(), std::inserter(new_lookup,std::end(new_lookup)),
		[&new_table = std::as_const(new_table)](const std::pair<std::string_view, size_t>& key) {
		std::string_view str(new_table.data() + key.second, key.first.size());
		return std::pair<std::string_view, size_t>(str, key.second);
	}
	);
	_table = std::move(new_table);
	_lookup = std::move(new_lookup);
	_currernt = reinterpret_cast<helper_t*>(_table.data() + _current_pos);
}

void string_builder::clear_mark() {
	_currernt->size = 0;
	_currernt = reinterpret_cast<helper_t*>(_table.data() + _current_pos);
}

void string_builder::push_char(char c) {
	if (_table.capacity() >= (_currernt->size + _current_pos + 1)) _resize();
	_currernt->str[++_currernt->size] = c;
}
void string_builder::pop_mark() {
	if (_currernt->size) --_currernt->size;
}

void string_builder::push(const char* str, size_t size) {
	if (_table.capacity() == (_currernt->size + size + 1)) _resize();
	std::copy_n(str, size, _currernt->str + _currernt->size);
	_currernt->size += static_cast<string_size_t>(size);
}


size_t string_builder::intern_mark() {
	_currernt->str[_currernt->size] = '\0';
	std::string_view str(_currernt->str, _currernt->size);
	size_t ret;
	auto it = _lookup.find(str);
	if (it != _lookup.end()) {
		clear_mark();
		ret = it->second;
	}
	else {
		ret = _current_pos;
		_lookup.emplace(str, _current_pos);
		_current_pos += static_cast<string_size_t>(_currernt->size) + 2;
		clear_mark();
	}
	return ret;
}


size_t string_builder::intern(const char* str, size_t size) {
	clear_mark();
	push(str, size);
	return intern_mark();
}

size_t string_builder::intern(const std::string_view& s) {
	clear_mark();
	push(s.data(),s.size());
	return intern_mark();
}

string_builder::string_builder() : _current_pos(0), _table(block_size) { intern("", 0); }

std::string_view string_builder::toString(string_t s) const {
	size_t index = s._handle & 0xFFFFFF;
	if (s._handle < 0x10000000 || index >= _table.size()) throw std::exception("Bad size for handle");
	const helper_t* ret = reinterpret_cast<const helper_t*>(_table.data() + index);
	return std::string_view(ret->str, ret->size);
}

#endif