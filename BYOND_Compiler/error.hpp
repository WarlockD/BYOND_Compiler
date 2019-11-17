#pragma once

#include "util.hpp"

// some stuff to make it easyer for me


#if 0


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
#endif

/*
	This is a hack of string view to offer intering so I don't have to rewerite all the functions for something
	that is already built into the standard.  I could just pass string view around but doing it this way
	makes me 100% for sure the string is internaned and safe rather than work with dangling pointers.
	Why not just use shard_ptr?  God knows how many strings we are dealing with.  

*/
#if 0

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

#endif