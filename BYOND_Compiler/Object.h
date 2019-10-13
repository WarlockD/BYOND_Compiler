#pragma once


#include <string_view>
#include <variant>
#include <memory>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <unordered_map>>

#include "util.hpp"
#include "entt.hpp"
//basicly evey object in there
namespace byond {
	using Name = string_t;
	using Path = std::filesystem::path;
	class Object;
	using ObjectType = std::filesystem::path;
	using ObjectInstance = std::shared_ptr<Object>;
	using SharedTexture = std::shared_ptr<sf::Texture>;

	using value_t = std::variant<std::nullptr_t, int, float, std::string, Name, Path, Icon,sf::Vector2i, ObjectType>;

	class Object : protected std::enable_shared_from_this<Object> {
		string_t _type;
		std::unordered_map<string_t, std::string> _procs;
		std::unordered_map<string_t, value_t> _local_vars;
		std::unordered_map<string_t, value_t> _static_vars;
		ObjectInstance _parent;
	public:
		string_t getTypeName() const { return _type; }
		value_t& get(const string_t& name) {
			if (auto it = _static_vars.find(name); it != _static_vars.end())
				return it->second;
			else if (auto it = _local_vars.find(name); it != _local_vars.end())
				return it->second;
			else std::nullptr_t;
		}
		const value_t& get(const string_t& name) const {
			if (auto it = _static_vars.find(name); it != _static_vars.end())
				return it->second;
			else if (auto it = _local_vars.find(name); it != _local_vars.end())
				return it->second;
			else std::nullptr_t;
		}
	};
	class Icon  {
		SharedTexture texture;
		sf::IntRect rect;
		string_t icon;
		string_t icon_state;
		int plane;
		sf::Color color;
		int _layer;

		int _dir;
		int plane;
		Name state;
	};


};