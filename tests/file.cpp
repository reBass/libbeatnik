#include <array>
#include <cstdio>
#include <fstream>
#include <gsl/gsl_assert>

#include <re/beatnik/beatnik.hpp>

int main(int argc, char* argv[])
{
	using beatnik = re::beatnik::beatnik<>;
	constexpr auto const step_size = beatnik::fft_step;
	constexpr auto const step_bytes = step_size * sizeof(beatnik::float_t);

	std::array<float, step_size> buffer;
	beatnik b(48000);

	Expects(argc >= 2);
	/* header-less sequence of mono f32 samples @ 48kHz */
	std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
	auto length = file.tellg();
	file.seekg(0);

	while(file.tellg() < length) {
		file.read(reinterpret_cast<char*>(&buffer[0]), step_bytes);
		if (file.gcount() != step_bytes) break;
		if (b.process(buffer)) { /* estimate ready */
			printf("%03.3f BPM\r", b.estimate_tempo());
			fflush(stdout);
		}

	}
	printf("%03.3f BPM\n\n", b.estimate_tempo());
	return 0;
}
