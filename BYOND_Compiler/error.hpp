#pragma once

#include "util.hpp"

// some stuff to make it easyer for me



class FileId {
	using fileid_t = uint16_t;
	fileid_t _value;
public:
	static constexpr fileid_t  FILEID_BUILTINS = 0x0000;
	static constexpr fileid_t  FILEID_MIN = 0x0001;
	static constexpr fileid_t  FILEID_MAX = 0xfffe;
	static constexpr fileid_t  FILEID_BAD = 0xffff;
	FileId() : _value(FILEID_BUILTINS) {}
	FileId(fileid_t v) : _value(v) {}
	bool bad() const { return _value == FILEID_BAD;  }

	operator fileid_t() const { return _value;}
};

struct Location {
	/// The index into the file table.
	union {
		struct {
			FileId _file;
			/// The column number, starting at 1.
			/// The line number, starting at 1.
			uint32_t _line;
			uint16_t _column;
		};
		uint64_t _packed;
	};

public:
	Location() : _file(), _column(1), _line(1) {}  
	Location(FileId file, uint16_t column, uint32_t line) : _file(file), _column(column), _line(line) {}
	Location(uint64_t packed) : _packed(packed) {}
	const uint32_t line() const { return _line; }
	const uint32_t column() const { return _column; }
	const FileId file() const { return _file; }
	const uint64_t packed() const { return _packed; }
	bool operator==(const Location& b) const { return _packed == b._packed; }
	bool operator>(const Location& b) const { return _file > b._file && _line > b._line && _column > b._column; }
};

static inline std::ostream& operator<<(std::ostream& os, const Location& l) {
	os << l.line() << ':' << l.column();
	return os;
}

enum class Severity {
	Error = 1,
	Warning = 2,
	Info = 3,
	Hint = 4,
};

enum class Color {
	None,
	Red,
	Yellow,
	White,
	BrightRead,
	BrightYellow,
	BrightWhite
};

class DMError {
	Location _location;
	Severity _severity;
	//component : Component,
	std::string _description;
	std::vector<std::pair<Location, std::string>> _notes;
public:
	DMError(Location l, std::string description, Severity s = Severity::Error) : _location(l), _description(description), _severity(s) {}
	void add_note(Location location, std::string message) { _notes.emplace_back(std::make_pair(location, message)); }
	const Location& location() const { return _location; }
	const std::string& description() const { return _description; }
	Severity severity() const { return _severity; }
};


static inline constexpr Color TextColor(Severity v) { 
	return
		Severity::Error == v ? Color::Red :
		Severity::Warning == v ? Color::Yellow :
		Severity::Info == v ? Color::White :
		Color::None;
}


template<typename T>
static constexpr std::string_view ToText(T v);

template<>
static inline constexpr std::string_view ToText(Severity v) {
	return
		Severity::Error == v ? "error" :
		Severity::Warning == v ? "warning" :
		Severity::Info == v ? "info" :
		"hint" ;
}

template<typename T>
static inline constexpr std::string_view ToText(T v) { static_assert(0, "Error, no to text"); }

//using string_t = std::string_view;
//std::shared_ptr<const std::string>;


/*
	This is a hack of string view to offer intering so I don't have to rewerite all the functions for something
	that is already built into the standard.  I could just pass string view around but doing it this way
	makes me 100% for sure the string is internaned and safe rather than work with dangling pointers.
	Why not just use shard_ptr?  God knows how many strings we are dealing with.  

*/


#if 0
class string_t : public std::string_view {
	struct _string_t {
		size_t size;
		char str[1];
		std::string_view strv() const { return std::string_view(str, size);  }
	};
	using table_t = std::unordered_map < std::string_view, std::unique_ptr<_string_t>>;
	static std::string_view _lookup(std::string_view n) {
		static table_t table;
		static std::mutex table_lock;
		std::string_view ret;
		table_t::const_iterator it;
		do {
			if ((it = table.find(n)) != table.end()) break;
			if (!table_lock.try_lock()) continue;
			if ((it = table.find(n)) != table.end()) {
				table_lock.unlock(); // this is in case the same word was chcked twice
				break;
			}
			size_t size = n.size() + sizeof(size_t);
			char* nstr = new char[size];
			std::copy(n.begin(), n.end(), nstr + sizeof(size_t));
			nstr[size] = 0;
			auto ns = std::unique_ptr<_string_t>(reinterpret_cast<_string_t*>(nstr));
			table.emplace(ns->strv(), std::move(ns));
			table_lock.unlock();
		} while (1);
		return it->second->strv();
	}
public:
	using base_type = std::string_view;
	using traits_type            = base_type::traits_type;
	using value_type             = base_type::value_type;
	using pointer                = base_type::pointer;
	using const_pointer          = base_type::const_pointer;
	using reference              = base_type::reference;
	using const_reference        = base_type::const_reference;
	using const_iterator         = base_type::const_iterator;
	using iterator               = base_type::iterator;
	using const_reverse_iterator = base_type::const_reverse_iterator;
	using reverse_iterator       = base_type::reverse_iterator;
	using size_type              = base_type::size_type;
	using difference_type        = base_type::difference_type;
	constexpr string_t() noexcept : std::string_view() {}
	string_t(const char* s) : std::string_view(_lookup(s)) {}
	string_t(const std::string_view& s) : std::string_view(_lookup(s)) {}
	string_t(const char* s,size_t n) : std::string_view(_lookup(std::string_view(s,n))) {}
	string_t(const std::string& s) : std::string_view(_lookup(s)) {}
	template<size_t N>
	constexpr string_t(const char(&v)[N]) : std::string_view(v) {} // don't need to intern for literals
	string_t(const string_t&) noexcept = default;
	string_t& operator=(const string_t&) noexcept = default;
	string_t& operator=(const std::string_view& v) noexcept { 	*this = string_t(v); return *this; }
	template<size_t N>
	constexpr string_t& operator=(const string_t&v) noexcept { *this = string_t(v); return *this; }
	using base_type::begin;
	using base_type::end;
	using base_type::cbegin;
	using base_type::cend;
	using base_type::rbegin;
	using base_type::rend;
	using base_type::crbegin;
	using base_type::crend;
	using base_type::size;
	using base_type::length;
	using base_type::empty;
	using base_type::data;
	using base_type::max_size;
	using base_type::operator[];
	using base_type::at;
	using base_type::front;
	using base_type::back;
	void swap(string_t& other) { std::swap<std::string_view>(*this, other); }
	using base_type::copy;
	using base_type::substr;
	using base_type::compare;
	
#if _HAS_CXX20
	using base_type::starts_with;
	using base_type::ends_with;
#endif

	using base_type::find;
	using base_type::rfind;
	using base_type::find_first_of;
	using base_type::find_last_of;
	using base_type::find_first_not_of;
	using base_type::find_last_not_of;
};

constexpr static inline bool operator==(const string_t& l, const string_t& r) { return l.data() == r.data(); }
constexpr static inline bool operator!=(const string_t& l, const string_t& r) { return l.data() != r.data(); }
constexpr static inline bool operator==(const string_t& l, const std::string_view& r) { return static_cast<const std::string_view>(l) == r; }
constexpr static inline bool operator==(const std::string_view& l, const string_t& r) { return l == static_cast<const std::string_view>(r); }
constexpr static inline bool operator!=(const string_t& l, const std::string_view& r) { return static_cast<const std::string_view>(l) != r; }
constexpr static inline bool operator!=(const std::string_view& l, const string_t& r) { return l != static_cast<const std::string_view>(r); }
#endif


class Context {
	using map_t = std::unordered_map<string_t, FileId> ;
	std::vector<string_t> _files;
	map_t reverse_files;
	std::vector<DMError> _errors;
	std::optional<Severity> _severity;


	FileId register_file(string_t path) {
		auto it = reverse_files.find(path);
		if (it != reverse_files.end()) return it->second;
		FileId next = (uint16_t)_files.size() + FileId::FILEID_MIN;
		reverse_files.insert(std::make_pair(path, next));
		_files.emplace_back(path);
		return next;
	}
	/// Look up a file's ID by its path, without inserting it.
	FileId get_file(string_t path) {
		auto it = reverse_files.find(path);
		if (it != reverse_files.end()) return it->second;
		else return FileId(FileId::FILEID_BAD);
	}
	/// Look up a file path by its index returned from `register_file`.
	std::optional<string_t> file_path(FileId id) {
		if (id == FileId::FILEID_BUILTINS) return string_t(".");
		if (id < _files.size()) return _files[id];
		return std::nullopt;
	}
	void pretty_print_error(std::ostream& os, const DMError& error) {
#if 0
		os << file_path(error.location().file()).value_or("blank")
			<< ", line " << error.location().line()
			<< ", column " << error.location().column() << ": ";
		os << error.description() << std::endl;

		for note in error.notes().iter() {
			if note.location == error.location{
				writeln!(w, "- {}", note.description,) ? ;
			}
			else if note.location.file == error.location.file{
			 writeln!(
				 w,
				 "- {}:{}: {}",
				 note.location.line,
				 note.location.column,
				 note.description,
			 ) ? ;
			}
			else {
				writeln!(
					w,
					"- {}:{}:{}: {}",
					self.file_path(note.location.file).display(),
					note.location.line,
					note.location.column,
					note.description,
					) ? ;
			}
		}
#endif
	}
	void register_error(DMError&& error) {
		pretty_print_error(std::cout,error);
		_errors.emplace_back(error);
	}
};