#define _USE_MATH_DEFINES

#include "image.h"
#include "stb_image.h"

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <algorithm>

Image::Image() : m_width(0), m_height(0)
{ }

Image::Image(int width, int height) : 
    m_width(width), 
    m_height(height), 
    m_pixels(width* height)
{ }

void Image::setSize(int width, int height)
{
    m_width = width;
    m_height = height;
    m_pixels.resize(m_width * m_height);
}

const glm::vec4& Image::getPixel(int i, int j) const
{
    return m_pixels[i + j * m_width];
}
	
const glm::vec4* Image::getPixels() const
{
    return m_pixels.data();
}

glm::vec4* Image::getPixels()
{
    return m_pixels.data();
}

void Image::setPixel(int i, int j, const glm::vec4& pixel)
{
    m_pixels[i + j * m_width] = pixel;
}

float const* Image::raw_data() const
{
    return reinterpret_cast<float const*>(m_pixels.data());
}

float* Image::raw_data()
{
    return reinterpret_cast<float*>(m_pixels.data());
}

void Image::clear(glm::vec4 const& color)
{
    for (int i = 0; i < m_width * m_height; i++)
    {
        m_pixels[i] = color;
    }
}

void Image::save(std::string const& path, float gamma) const
{
	size_t lastindex = path.find_last_of(".");
	const std::string extension = path.substr(lastindex + 1, path.length());

	std::vector<std::uint8_t> bgr(m_pixels.size() * 3);

	for (int y = 0; y < m_height; ++y)
		for (int x = 0; x < m_width; ++x)
		{
			std::size_t const idx_dst = 3 * ((m_height - y - 1) * m_width + x);
			std::size_t const idx_src = (y * m_width + x);

			for (int c = 0; c < 3; ++c)
			{
				float const gamma_corrected = std::pow(std::max(0.f, m_pixels[idx_src][c]), 1.f / gamma);
				float const mapped = std::max<float>(0.0f, std::min<float>(255.0f, 255.f * gamma_corrected));
				bgr[idx_dst + c] = uint8_t(mapped);
			}
		}
	if (extension == "tga");
	else if (extension == "png");
	else;
}

void Image::load(std::string const& path, float gamma)
{
	int num_components;
	stbi_ldr_to_hdr_gamma(gamma);
    float *data = stbi_loadf(path.c_str(), &m_width, &m_height, &num_components, 4);
	if(!data) {
		std::cerr << "error: could not load image \"" << path << "\"" << std::endl;
    m_width = m_height = 1;
    m_pixels.resize(1);
		return;
	}
	m_pixels.resize(m_width * m_height);
	/* flip image in Y */
	for(int y = 0; y < m_height; y++) {
		memcpy(&m_pixels[(m_height - y - 1) * m_width],
				data + y * m_width * 4,
				4 * m_width * sizeof(float));
	}
	stbi_image_free(data);
}

void Image::save_pfm(std::string const& path) const
{
	std::ofstream of(path.c_str(), std::ios::out | std::ios::binary);

	if (!of) {
		std::cerr << "Cannot open " << path << " for writing." << std::endl;
		return;
	}

	// write header.
	of << "PF\n" << m_width << " " << m_height << "\n-1.0\n" << std::flush;

	std::vector<float> data_pfm(m_width * m_height * 3);
	for (int j = 0; j < m_width * m_height; ++j) {
		for (int i = 0; i < 3; ++i) {
			data_pfm[3*j+i] = getPixels()[j][i];
		}
	}
	of.write(reinterpret_cast<char const*>(&data_pfm[0]), 
		static_cast<std::streamsize>(m_width * m_height * 3 * sizeof(float)));

	if (!of) {
		std::cerr << "An error occured while writing " << path << std::endl;
	}

	of.close();
}
	
static void readCommentsAndEmptyLines(std::ifstream& file)
{
	std::string line;
	while (true) {
		std::streampos pos = file.tellg();
		getline(file, line);
		if (!(line.length() > 0 && line.at(0) == '#')) {
			file.seekg(pos);
			break;
		}
	}
}

void Image::load_pfm(std::string const& path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file || !file.is_open()) {
		std::cerr << "Cannot open " << path << " for reading." << std::endl;
		return;
	}

	std::string line;

	// read file type
	getline(file, line);
	readCommentsAndEmptyLines(file);

	// read width, height
	getline(file, line);
	sscanf_s(line.c_str(), "%i %i", &m_width, &m_height);
	getline(file, line); // read endian crap
	float endian;
	sscanf_s(line.c_str(), "%f", &endian);
	if (endian > 0) {
		std::cerr << "only little endian supported" << std::endl;
		m_width = 0;
		m_height = 0;
		return;
	}

	std::vector<float> data_pfm(m_width * m_height * 3 * sizeof(float));
	file.read(reinterpret_cast<char*>(&data_pfm[0]), 
		static_cast<std::streamsize>(sizeof(float) * 3 * m_width * m_height));
	if (!file) {
		std::cerr << "An error occured while reading " << path << "." << std::endl;
		return;
	}
	m_pixels.resize(m_width*m_height);

	for (int y = 0; y < m_height; ++y) 
	for (int x = 0; x < m_width; ++x) 
	{
		int offset = x + y * m_width;
		m_pixels[offset] = glm::vec4(
			data_pfm[3 * offset + 0],
			data_pfm[3 * offset + 1],
			data_pfm[3 * offset + 2],
			0.f);
	}
	file.close();
}

void Image::tonemap_01(float exposure, float gamma)
{
	glm::vec4 maxval {0.f, 0.f, 0.f, 0.f};
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			maxval = max(getPixel(x, y), maxval);
		}
	}
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			setPixel(x, y, pow(std::pow(2.f, exposure) * getPixel(x, y) / maxval, glm::vec4{1.f/gamma}));
		}
	}
}

/*
 * Writes a buffer of complex numbers into an image.
 *
 * - If rgba = true, the imaginary parst will be ignored and the real
 *	 part will be written to create an greyscale rgba image
 *
 * - If rgba = false, the data will be interpreted as an fourier spectrum
 *   and the real and imaginary parts will be written into the R channel and
 *   G channel respecively
 */
void Image::complex_to_image(std::vector<std::complex<float>> const& vec, Image* img, bool rgba)
{
	for (int j = 0; j < int(vec.size()); ++j)
	{
		const float r = vec[j].real();
		const float i = vec[j].imag();
		const float v = std::sqrt(r*r+i*i);
		if (rgba)
			img->getPixels()[j] = glm::vec4(v, v, v, 1.0f);
		else
			img->getPixels()[j] = glm::vec4(r, i, 0.0f, 1.0f);
	}
}
		
/*
 * Reads a buffer of complex numbers from an image.
 *
 * - If rgba = true, the color information of the image 
 *   will be averaged and written into the real part
 *   of the buffer.
 *
 * - If rgba = false, the data will be interpreted as an fourier spectrum
 *   and the R and G channel will be written into the real and imaginary
 *   part of the buffer respecively
 */
void Image::image_to_complex(Image const& img, std::vector<std::complex<float>>* vec, bool rgba)
{
	for (int i = 0; i < int(vec->size()); ++i)
	{
		glm::vec4 const& d = img.getPixels()[i];
		if (rgba)
			(*vec)[i] = std::complex<float>((d[0]+d[1]+d[2])/3.f, 0);
		else
			(*vec)[i] = std::complex<float>(d[0], d[1]);
	}
}

glm::vec4 Image::getPixel(int i, int j, WrapMode wrap_mode) const
{
	switch (wrap_mode) {
		case ZERO:
			if (i < 0 || i >= m_width || j < 0 || j >= m_height)
				return glm::vec4(0.f);
		break;
		case CLAMP:
			i = std::min(m_width-1,  std::max(0, i));
			j = std::min(m_height-1, std::max(0, j));
		break;
		case REPEAT:
			while (i < 0)
				i += m_width;
			i = i % m_width;
			while (j < 0)
				j += m_height;
			j = j % m_height;
		break;
	}
	return m_pixels[i + j * m_width];
}

void Image::filter_gaussian(Image *target, float sigma, int kernel_size, WrapMode wrap_mode) const
{
	std::vector<float> kernel(kernel_size*kernel_size);
	create_gaussian_kernel_2d(sigma, kernel_size, kernel.data());
	filter(target, kernel_size, kernel.data(), wrap_mode);
}

void Image::filter_gaussian_separable(Image *target, float sigma, int kernel_size, WrapMode wrap_mode) const
{
	std::vector<float> kernel(kernel_size);
	create_gaussian_kernel_1d(sigma, kernel_size, kernel.data());
	filter_separable(target, kernel_size, kernel.data(), wrap_mode);
}

/*
 * Create a 1 dimensional normalized gauss kernel
 *
 * Parameters:
 *  - sigma:       the parameter of the gaussian
 *  - kernel_size: the size of the kernel (has to be odd)
 *  - kernel:      an array with size kernel_size elements that
 *                 has to be filled with the kernel values
 *
 */
void 
Image::create_gaussian_kernel_1d(
		float sigma,
		int kernel_size,
		float* kernel)
{
	
	// TODO: calculate filter values as described on the exercise sheet. 
	// Make sure your kernel is normalized
	const float sigma_squared = sigma*sigma;
	float normalize = 0.f;
	for (int i = -kernel_size/2; i <= kernel_size/2; ++i) {
		const float value = std::exp(-(i*i)/(2*sigma_squared));
		normalize += value;
		kernel[i+kernel_size/2] = value;
	}

	for (int i = 0; i < kernel_size; ++i) {
		kernel[i] /= normalize;
	}
}

/*
 * Create a 2 dimensional quadratic and normalized gauss kernel
 *
 * Parameters:
 *  - sigma:       the parameter of the gaussian
 *  - kernel_size: the size of the kernel (has to be odd)
 *  - kernel:      an array with kernel_size*kernel_size elements that
 *                 has to be filled with the kernel values
 */
void 
Image::create_gaussian_kernel_2d(
		float sigma,
		int kernel_size,
		float* kernel)
{

	// TODO: calculate filter values as described on the exercise sheet. 
	// Make sure your kernel is normalized
	const float sigma_squared = sigma*sigma;
	float normalize = 0.f;
	for (int j = -kernel_size/2; j <= kernel_size/2; ++j) {
		for (int i = -kernel_size/2; i <= kernel_size/2; ++i) {
			const float value = std::exp(-(i*i+j*j)/(2*sigma_squared));
			normalize += value;
			kernel[(i+kernel_size/2)+(j+kernel_size/2)*kernel_size] = value;
		}
	}

	for (int j = 0; j < kernel_size; ++j) {
		for (int i = 0; i < kernel_size; ++i) {
			kernel[i+j*kernel_size] /= normalize;
		}
	}
}
	
/*
 * Convolve an image with a 2d filter kernel
 *
 * Parameters:
 *  - kernel_size: the size of the 2d-kernel
 *  - kernel:      the 2d-kernel with kernel_size*kernel_size elements
 *  - wrap_mode:   needs to be known to handle repeating 
 *                 textures correctly
 */
void Image::
filter(Image *target, int kernel_size, float* kernel, WrapMode wrap_mode) const
{
	// TODO: perform the naive 2d convolution here. 
	// use the methods getPixel(x, y, wrap_mode) and
	// setPixel(x, y, value) to get and set pixels of an image
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			glm::vec4 val(0.f);
			for (int j = -kernel_size/2; j <= kernel_size/2; ++j) {
				for (int i = -kernel_size/2; i <= kernel_size/2; ++i) {
					const float weight = kernel[(i+kernel_size/2)+(j+kernel_size/2)*kernel_size];
					val += weight * getPixel(x+i, y+j, wrap_mode);
				}
			}
			target->setPixel(x, y, val);
		}
	}

}

/*
 * Convolve an image with a separable 1d filter kernel
 *
 * Parameters:
 *  - kernel_size: the size of the 1d kernel
 *  - kernel:      the 1d-kernel with kernel_size elements
 *  - wrap_mode:   needs to be known to handle repeating 
 *                 textures correctly
 */
void Image::
filter_separable(Image *target, int kernel_size, float* kernel, WrapMode wrap_mode) const
{
	// TODO: realize the 2d convolution with two
	// convolutions of the image with a 1d-kernel.
	// convolve the image horizontally and then convolve
	// the result vertically (or vise-versa).
	//
	// use the methods getPixel(x, y, wrap_mode) and
	// setPixel(x, y, value) to get and set pixels of an image
	Image tmp_image(m_width, m_height);

	// filter in y direction
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			glm::vec4 val(0.f);
			for (int dy = -kernel_size/2; dy <= kernel_size/2; ++dy) {
				const float weight = kernel[dy+kernel_size/2];
				val += weight * getPixel(x, y+dy, wrap_mode);
			}
			tmp_image.setPixel(x, y, val);
		}
	}
	// filter in x direction
	for (int y = 0; y < m_height; ++y)
	{
		for (int x = 0; x < m_width; ++x)
		{
			glm::vec4 val(0.f);
			for (int dx = -kernel_size/2; dx <= kernel_size/2; ++dx) {
				const float weight = kernel[dx+kernel_size/2];
				val += weight * tmp_image.getPixel(x+dx, y, wrap_mode);
			}
			target->setPixel(x, y, val);
		}
	}
}

void DiscreteFourier2D::reconstruct(
	int M, int N,
	std::complex<float> const* spectrum,
	std::complex<float>	* reconstruction)
{
	// separated transform
	std::vector<std::complex<float>> tmp(M*N, std::complex<float>(0.0f, 0.0f));
	for (int l = 0; l < N; ++l) // row
	for (int k = 0; k < M; ++k) // column
	{
		// transformation over column
		std::complex<float> x_kl = 0;
		for (int n = 0; n < N; ++n) 
		{
			std::complex<float> exponent { 0, 
				2.f * float(M_PI) * l * (float(n)/N - 0.5f) };
			x_kl += spectrum[k+n*M] * std::exp(exponent);
			
		}
		tmp[k+l*M] = x_kl;
	}
	for (int l = 0; l < N; ++l) // row
	for (int k = 0; k < M; ++k) // column
	{
		// transformation over row
		std::complex<float> x_kl = 0;
		for (int m = 0; m < M; ++m)
		{
			std::complex<float> exponent { 0, 
				2.f * float(M_PI) * k * (float(m)/M - 0.5f) };
			x_kl += tmp[m+l*M] * std::exp(exponent);
			
		}
		reconstruction[k+l*M] = 1.f/std::sqrt(float(M*N)) * x_kl;
	}
}

