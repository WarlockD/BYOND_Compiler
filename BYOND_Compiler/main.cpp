#include "error.hpp"

//#include "ExprParser.h"
//#include "error.hpp"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include "util.hpp"
#include "lodepng.h"
#include <iostream>
#include "Lex.hpp"
#include <tao/pegtl/contrib/tracer.hpp>

/*
Display general info about the PNG.
*/
void displayPNGInfo(const LodePNGInfo& info) {
	const LodePNGColorMode& color = info.color;

	std::cout << "Compression method: " << info.compression_method << std::endl;
	std::cout << "Filter method: " << info.filter_method << std::endl;
	std::cout << "Interlace method: " << info.interlace_method << std::endl;
	std::cout << "Color type: " << color.colortype << std::endl;
	std::cout << "Bit depth: " << color.bitdepth << std::endl;
	std::cout << "Bits per pixel: " << lodepng_get_bpp(&color) << std::endl;
	std::cout << "Channels per pixel: " << lodepng_get_channels(&color) << std::endl;
	std::cout << "Is greyscale type: " << lodepng_is_greyscale_type(&color) << std::endl;
	std::cout << "Can have alpha: " << lodepng_can_have_alpha(&color) << std::endl;
	std::cout << "Palette size: " << color.palettesize << std::endl;
	std::cout << "Has color key: " << color.key_defined << std::endl;
	if (color.key_defined) {
		std::cout << "Color key r: " << color.key_r << std::endl;
		std::cout << "Color key g: " << color.key_g << std::endl;
		std::cout << "Color key b: " << color.key_b << std::endl;
	}
	std::cout << "Texts: " << info.text_num << std::endl;
	for (size_t i = 0; i < info.text_num; i++) {
		std::cout << "Text: " << info.text_keys[i] << ": " << info.text_strings[i] << std::endl << std::endl;
	}
	std::cout << "International texts: " << info.itext_num << std::endl;
	for (size_t i = 0; i < info.itext_num; i++) {
		std::cout << "Text: "
			<< info.itext_keys[i] << ", "
			<< info.itext_langtags[i] << ", "
			<< info.itext_transkeys[i] << ": "
			<< info.itext_strings[i] << std::endl << std::endl;
	}
	std::cout << "Time defined: " << info.time_defined << std::endl;
	if (info.time_defined) {
		const LodePNGTime& time = info.time;
		std::cout << "year: " << time.year << std::endl;
		std::cout << "month: " << time.month << std::endl;
		std::cout << "day: " << time.day << std::endl;
		std::cout << "hour: " << time.hour << std::endl;
		std::cout << "minute: " << time.minute << std::endl;
		std::cout << "second: " << time.second << std::endl;
	}
	std::cout << "Physics defined: " << info.phys_defined << std::endl;
	if (info.phys_defined) {
		std::cout << "physics X: " << info.phys_x << std::endl;
		std::cout << "physics Y: " << info.phys_y << std::endl;
		std::cout << "physics unit: " << info.phys_unit << std::endl;
	}
}


/*
Display the names and sizes of all chunks in the PNG file.
*/
void displayChunkNames(const std::vector<unsigned char>& buffer) {
	// Listing chunks is based on the original file, not the decoded png info.
	const unsigned char* chunk, * begin, * end, * next;
	end = &buffer.back() + 1;
	begin = chunk = &buffer.front() + 8;

	std::cout << std::endl << "Chunks:" << std::endl;
	std::cout << " type: length(s)";
	std::string last_type;
	while (chunk + 8 < end && chunk >= begin) {
		char type[5];
		lodepng_chunk_type(type, chunk);
		if (std::string(type).size() != 4) {
			std::cout << "this is probably not a PNG" << std::endl;
			return;
		}

		if (last_type != type) {
			std::cout << std::endl;
			std::cout << " " << type << ": ";
		}
		last_type = type;

		std::cout << lodepng_chunk_length(chunk) << ", ";

		next = lodepng_chunk_next_const(chunk);
		if (next <= chunk) break; // integer overflow
		chunk = next;
	}
	std::cout << std::endl;
}


/*
Show ASCII art preview of the image
*/
void displayAsciiArt(const std::vector<unsigned char>& image, unsigned w, unsigned h) {
	if (w > 0 && h > 0) {
		std::cout << std::endl << "ASCII Art Preview: " << std::endl;
		unsigned w2 = 48;
		if (w < w2) w2 = w;
		unsigned h2 = h * w2 / w;
		h2 = (h2 * 2) / 3; //compensate for non-square characters in terminal
		if (h2 > (w2 * 2)) h2 = w2 * 2; //avoid too large output

		std::cout << '+';
		for (unsigned x = 0; x < w2; x++) std::cout << '-';
		std::cout << '+' << std::endl;
		for (unsigned y = 0; y < h2; y++) {
			std::cout << "|";
			for (unsigned x = 0; x < w2; x++) {
				unsigned x2 = x * w / w2;
				unsigned y2 = y * h / h2;
				int r = image[y2 * w * 4 + x2 * 4 + 0];
				int g = image[y2 * w * 4 + x2 * 4 + 1];
				int b = image[y2 * w * 4 + x2 * 4 + 2];
				int a = image[y2 * w * 4 + x2 * 4 + 3];
				int lightness = ((r + g + b) / 3) * a / 255;
				int min = (r < g && r < b) ? r : (g < b ? g : b);
				int max = (r > g && r > b) ? r : (g > b ? g : b);
				int saturation = max - min;
				int letter = 'i'; //i for grey, or r,y,g,c,b,m for colors
				if (saturation > 32) {
					int h = lightness >= (min + max) / 2;
					if (h) letter = (min == r ? 'c' : (min == g ? 'm' : 'y'));
					else letter = (max == r ? 'r' : (max == g ? 'g' : 'b'));
				}
				int symbol = ' ';
				if (lightness > 224) symbol = '@';
				else if (lightness > 128) symbol = letter - 32;
				else if (lightness > 32) symbol = letter;
				else if (lightness > 16) symbol = '.';
				std::cout << (char)symbol;
			}
			std::cout << "|";
			std::cout << std::endl;
		}
		std::cout << '+';
		for (unsigned x = 0; x < w2; x++) std::cout << '-';
		std::cout << '+' << std::endl;
	}
}


/*
Show the filtertypes of each scanline in this PNG image.
*/
void displayFilterTypes(const std::vector<unsigned char>& buffer, bool ignore_checksums) {
	//Get color type and interlace type
	lodepng::State state;
	if (ignore_checksums) {
		state.decoder.ignore_crc = 1;
		state.decoder.zlibsettings.ignore_adler32 = 1;
	}
	unsigned w, h;
	unsigned error;
	error = lodepng_inspect(&w, &h, &state, &buffer[0], buffer.size());

	if (error) {
		std::cout << "inspect error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	if (state.info_png.interlace_method == 1) {
		std::cout << "showing filtertypes for interlaced PNG not supported by this example" << std::endl;
		return;
	}

	//Read literal data from all IDAT chunks
	const unsigned char* chunk, * begin, * end, * next;
	end = &buffer.back() + 1;
	begin = chunk = &buffer.front() + 8;

	std::vector<unsigned char> zdata;

	while (chunk + 8 < end && chunk >= begin) {
		char type[5];
		lodepng_chunk_type(type, chunk);
		if (std::string(type).size() != 4) {
			std::cout << "this is probably not a PNG" << std::endl;
			return;
		}

		if (std::string(type) == "IDAT") {
			const unsigned char* cdata = lodepng_chunk_data_const(chunk);
			unsigned clength = lodepng_chunk_length(chunk);
			if (chunk + clength + 12 > end || clength > buffer.size() || chunk + clength + 12 < begin) {
				std::cout << "invalid chunk length" << std::endl;
				return;
			}

			for (unsigned i = 0; i < clength; i++) {
				zdata.push_back(cdata[i]);
			}
		}

		next = lodepng_chunk_next_const(chunk);
		if (next <= chunk) break; // integer overflow
		chunk = next;
	}

	//Decompress all IDAT data
	std::vector<unsigned char> data;
	error = lodepng::decompress(data, &zdata[0], zdata.size());

	if (error) {
		std::cout << "decompress error " << error << ": " << lodepng_error_text(error) << std::endl;
		return;
	}

	//A line is 1 filter byte + all pixels
	size_t linebytes = 1 + lodepng_get_raw_size(w, 1, &state.info_png.color);

	if (linebytes == 0) {
		std::cout << "error: linebytes is 0" << std::endl;
		return;
	}

	std::cout << "Filter types: ";
	for (size_t i = 0; i < data.size(); i += linebytes) {
		std::cout << (int)(data[i]) << " ";
	}
	std::cout << std::endl;

}
struct metadata {


	size_t width;
	size_t height;
	std::vector<size_t> states;
	std::unordered_map<string_t, size_t> state_names;

	//void parse_metadata(std::istream& is) {
	//	std::string line;
	//	if (std::getline(is, line) && line.find_first_of("# BEGIN DMI")) {
	//
//
	//}
};
namespace dmi_parser {
	enum class DIR {
		NORTH = 1,
		SOUTH= 2,
		EAST  = 4,
		WEST = 8,
		NORTHEAST = 5,
		NORTHWEST = 9,
		SOUTHEAST  = 6,
		SOUTHWEST  = 10,
	};
		/// Without an explicit setting, only one frame
		/// There are this many frames lasting one tick each
		//Count(usize),
		/// Each frame lasts the corresponding number of ticks
		//Delays(Vec<f32>),
		// TODO: hotspot support here
	using frames_t = std::variant<std::nullptr_t, size_t, std::vector<float>>;
	enum class DIRS {
		One = 1,
		Four = 4,
		Eight = 8
	};
	struct state_t {
		/// Frames before this state starts
		size_t offset;
		string_t name;
		size_t loop;
		bool rewind;
		bool movement;
		DIRS dirs;
		frames_t frames;
		size_t frame_count() const {
			return 				
				std::visit(overloaded{  // (3)
					[](std::nullptr_t) ->size_t { return 1; },
					[](size_t i)->size_t { return i; },
					[](const std::vector<float>& times)->size_t { return times.size(); }
				}, frames);
		}
		float frame_delay(size_t idx) const {
			return
				std::visit(overloaded{  // (3)
					[idx](std::nullptr_t) ->float { return 1.0f; },
					[idx](size_t i)->float { return 1.0f; },
					[idx](const std::vector<float>& times)->float { return times[idx]; }
					}, frames);
		}
		size_t sprite_count() const {
			return static_cast<size_t>(dirs)* frame_count();
		}
	};

	struct metadata_t {
		size_t width;
		size_t height;
		std::vector<state_t> states;
		std::unordered_map<string_t, size_t> state_names;
		metadata_t() : width(32), height(32) {}
	};


	using namespace TAO_PEGTL_NAMESPACE;
	// clang-format off
	struct blank0 : star< blank > {};
	struct blanks : plus< blank > {};
	struct number_value : plus< digit > {};
	struct string_value :seq < one<'"'>, until<one<'"'>>> {};
	struct value : sor<number_value, string_value> {};
	struct delay_time : number {};

	struct name : identifier {};
	struct equals : pad< one< '=' >, blanks > {};
	// 
	template<typename NAME,typename VALUE>
	struct assignment : if_must< NAME, equals, VALUE> {};

	struct delay_item : pad< delay_time, blank > {};
	struct delay_list : list_must< delay_item, one< ',' > > {};

	struct dmi_delay : assignment< TAO_PEGTL_STRING("delay"), delay_list> {};
	struct dmi_width : assignment< TAO_PEGTL_STRING("width"), number_value> {};
	struct dmi_height : assignment< TAO_PEGTL_STRING("height"), number_value> {};
	struct dmi_dirs : assignment< TAO_PEGTL_STRING("dirs"), number_value> {};
	struct dmi_frames : assignment< TAO_PEGTL_STRING("frames"), number_value> {};
	struct dmi_state : assignment< TAO_PEGTL_STRING("state"), string_value> {};
	struct dmi_loop : assignment< TAO_PEGTL_STRING("loop"), number_value> {};
	struct dmi_hotspot : assignment< TAO_PEGTL_STRING("hotspot"), number_value> {};
	struct dmi_rewind : assignment< TAO_PEGTL_STRING("rewind"), number_value> {};
	struct dmi_movement : assignment< TAO_PEGTL_STRING("movement"), number_value> {};


	struct assignments : pad<sor< dmi_width, dmi_height, dmi_dirs, dmi_frames, dmi_state, dmi_loop, dmi_hotspot, dmi_rewind, dmi_movement>, blanks>{};

	struct END_OF_DMI : pad<TAO_PEGTL_STRING("# END DMI"),blanks> {};
	struct BEGIN_OF_DMI : pad < TAO_PEGTL_STRING("# BEGIN DMI"), blanks> {};
	struct VERSION_OF_DMI : pad < TAO_PEGTL_STRING("version = 4.0"), blanks> {};

	struct start_of_dmi : seq<BEGIN_OF_DMI, eol, VERSION_OF_DMI, eol> {};
	struct end_of_dmi : sor<END_OF_DMI, eof> {};
	struct dmi_grammer : seq <start_of_dmi, until<end_of_dmi, must<seq<assignments,eol>>>> {};


	struct state {
		std::string name;
		std::variant<std::string,int,std::vector<float>> value;
		metadata_t meta;
		size_t frames_so_far;
		state() :frames_so_far(0) {}
	};

	template< typename Rule >
	struct action
	{};
	template<>
	struct action< string_value >
	{
		template< typename Input >
		static void apply(const Input& in, state& st)
		{
			st.value = in.string();
#if 0
			if (st.temp == "state") {
				if (st.meta.states.size() > 0) {
					st.frames_so_far += st.meta.states.back().frame_count();
				}
				string_t name = text;
				st.meta.state_names[name] = st.meta.states.size();
				st.meta.states.push_back(state_t());
				st.meta.states.back().offset = st.frames_so_far;
				st.meta.states.back().name = name;
			}
#endif
		}
	};
	template<>
	struct action< delay_time >
	{
		template< typename Input >
		static void apply(const Input& in, state& st)
		{
			if (!std::holds_alternative < std::vector<float>(st.value)) {
				st.value = std::move(std::vector<float>());
			}
			std::get< std::vector<float>>(st.value).push_back(strtof(in.string().c_str(), NULL));
#if 0
			if (st.temp == "width") {
				st.meta.width
			}
			else if (st.temp == "height") {
				st.meta.height = strtol(text.c_str(), NULL, 10);
			}
#endif

		}
	};
	template<>
	struct action< number_value >
	{
		template< typename Input >
		static void apply(const Input& in, state& st)
		{
			st.value = static_cast<int>(strtol(in.string().c_str(), NULL, 10));
#if 0
			if (st.temp == "width") {
				st.meta.width 
			}
			else if (st.temp == "height") {
				st.meta.height=strtol(text.c_str(), NULL, 10);
			}
#endif

		}
	};
	template<>
	struct action< name >
	{
		template< typename Input >
		static void apply(const Input& in, state& st)
		{
			st.name = in.string();
		}
	};


};
namespace pegtl = TAO_PEGTL_NAMESPACE;
//Lexer lexer;
int main(int argc, const char** argv)
{
	bool ignore_checksums = false;
	std::string filename = "";
	for (int i = 1; i < argc; i++) {
		if (std::string(argv[i]) == "--ignore_checksums") ignore_checksums = true;
		else filename = argv[i];
	}
	if (filename == "") {
		std::cout << "Please provide a filename to preview" << std::endl;
		return 0;
	}

	std::vector<unsigned char> buffer;
	std::vector<unsigned char> image;
	unsigned w, h;

	lodepng::load_file(buffer, filename); //load the image file with given filename

	lodepng::State state;
	if (ignore_checksums) {
		state.decoder.ignore_crc = 1;
		state.decoder.zlibsettings.ignore_adler32 = 1;
	}

	unsigned error = lodepng::decode(image, w, h, state, buffer);

	if (error) {
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		return 0;
	}

	std::cout << "Filesize: " << buffer.size() << " (" << buffer.size() / 1024 << "K)" << std::endl;
	std::cout << "Width: " << w << std::endl;
	std::cout << "Height: " << h << std::endl;
	std::cout << "Num pixels: " << w * h << std::endl;

	if (w > 0 && h > 0) {
		std::cout << "Top left pixel color:"
			<< " r: " << (int)image[0]
			<< " g: " << (int)image[1]
			<< " b: " << (int)image[2]
			<< " a: " << (int)image[3]
			<< std::endl;
	}
	auto& info = state.info_png;
	std::unordered_map<std::string, std::string> text_data;
	for (size_t i = 0; i < info.text_num; i++) {
		text_data[info.text_keys[i]] = info.text_strings[i];
	}

	for (auto p : text_data) {
		if (p.first == "Description") {
			for (int i = 1; i < argc; ++i) {
				std::string str = p.second;
				pegtl::string_input si(str,"erwhat?");
				dmi_parser::state st;
				pegtl::parse< dmi_parser::dmi_grammer, dmi_parser::action,pegtl::tracer >(si, st);
			//	pegtl::parse< dmi_parser::dmi_grammer>(si, st);
#if 0
				for (const auto& j : st.symbol_table) {
					std::cout << j.first << " = " << j.second << std::endl;
				}
#endif
			}



		}
	}
	//eturn byond_compiler::preprocessor::test_it(argc, argv);

#if 0
	 // Request a 24-bits depth buffer when creating the window
	sf::ContextSettings contextSettings;
	contextSettings.depthBits = 24;

	// Create the main window
	sf::Window window(sf::VideoMode(640, 480), "SFML window with OpenGL", sf::Style::Default, contextSettings);

	// Make it the active window for OpenGL calls
	window.setActive();

	// Set the color and depth clear values
	glClearDepth(1.f);
	glClearColor(0.f, 0.f, 0.f, 1.f);

	// Enable Z-buffer read and write
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	// Disable lighting and texturing
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// Configure the viewport (the same size as the window)
	glViewport(0, 0, window.getSize().x, window.getSize().y);

	// Setup a perspective projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat ratio = static_cast<float>(window.getSize().x) / window.getSize().y;
	glFrustum(-ratio, ratio, -1.f, 1.f, 1.f, 500.f);

	// Define a 3D cube (6 faces made of 2 triangles composed by 3 vertices)
	GLfloat cube[] =
	{
		// positions    // colors (r, g, b, a)
		-50, -50, -50,  0, 0, 1, 1,
		-50,  50, -50,  0, 0, 1, 1,
		-50, -50,  50,  0, 0, 1, 1,
		-50, -50,  50,  0, 0, 1, 1,
		-50,  50, -50,  0, 0, 1, 1,
		-50,  50,  50,  0, 0, 1, 1,

		 50, -50, -50,  0, 1, 0, 1,
		 50,  50, -50,  0, 1, 0, 1,
		 50, -50,  50,  0, 1, 0, 1,
		 50, -50,  50,  0, 1, 0, 1,
		 50,  50, -50,  0, 1, 0, 1,
		 50,  50,  50,  0, 1, 0, 1,

		-50, -50, -50,  1, 0, 0, 1,
		 50, -50, -50,  1, 0, 0, 1,
		-50, -50,  50,  1, 0, 0, 1,
		-50, -50,  50,  1, 0, 0, 1,
		 50, -50, -50,  1, 0, 0, 1,
		 50, -50,  50,  1, 0, 0, 1,

		-50,  50, -50,  0, 1, 1, 1,
		 50,  50, -50,  0, 1, 1, 1,
		-50,  50,  50,  0, 1, 1, 1,
		-50,  50,  50,  0, 1, 1, 1,
		 50,  50, -50,  0, 1, 1, 1,
		 50,  50,  50,  0, 1, 1, 1,

		-50, -50, -50,  1, 0, 1, 1,
		 50, -50, -50,  1, 0, 1, 1,
		-50,  50, -50,  1, 0, 1, 1,
		-50,  50, -50,  1, 0, 1, 1,
		 50, -50, -50,  1, 0, 1, 1,
		 50,  50, -50,  1, 0, 1, 1,

		-50, -50,  50,  1, 1, 0, 1,
		 50, -50,  50,  1, 1, 0, 1,
		-50,  50,  50,  1, 1, 0, 1,
		-50,  50,  50,  1, 1, 0, 1,
		 50, -50,  50,  1, 1, 0, 1,
		 50,  50,  50,  1, 1, 0, 1,
	};

	// Enable position and color vertex components
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 7 * sizeof(GLfloat), cube);
	glColorPointer(4, GL_FLOAT, 7 * sizeof(GLfloat), cube + 3);

	// Disable normal and texture coordinates vertex components
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Create a clock for measuring the time elapsed
	sf::Clock clock;

	// Start the game loop
	while (window.isOpen())
	{
		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			// Close window: exit
			if (event.type == sf::Event::Closed)
				window.close();

			// Escape key: exit
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();

			// Resize event: adjust the viewport
			if (event.type == sf::Event::Resized)
				glViewport(0, 0, event.size.width, event.size.height);
		}

		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Apply some transformations to rotate the cube
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.f, 0.f, -200.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 50, 1.f, 0.f, 0.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 30, 0.f, 1.f, 0.f);
		glRotatef(clock.getElapsedTime().asSeconds() * 90, 0.f, 0.f, 1.f);

		// Draw the cube
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// Finally, display the rendered frame on screen
		window.display();
	}
#endif
	return EXIT_SUCCESS;
}
