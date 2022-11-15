#include "GLObjModel.h"
#include "obj_loader.h"
#include "image.h"

#include <iostream>
#include <map>
#include <unordered_map>
#include <stdexcept>

struct GLTextureList
{
	std::map<std::string, unsigned int> textureIDs;

	GLTextureList() :
		textureIDs() {}

	unsigned int getGLTexture(const std::string& filename, float gamma = 1.0f);
};

unsigned int GLTextureList::
getGLTexture(const std::string& filename, float gamma)
{
	const auto& i = textureIDs.find(filename);

	// already loaded?
	if (i != textureIDs.end())
		return i->second;

	std::string ending = "";
	for (int i = filename.size() - 1; i >= 0; i--) {
		if (filename.c_str()[i] == '.')
			break;
		else
			ending = filename.c_str()[i] + ending;
	}
	unsigned int tex;
	if (ending.compare("dds") == 0) {
		tex = texture_loadDDS(filename.c_str());
	}
	else {
		tex = loadGLTexture(filename, gamma);
	}
	textureIDs[filename] = tex;

	return tex;
}

unsigned int
texture_loadDDS(const char* path) {
	nv_dds::CDDSImage image;
	unsigned int tex = 0;
	try
	{
		image.load(path);
	}
	catch (const std::runtime_error& error)
	{
		std::cout << error.what() << "\n";
		return -1;
	}
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	image.upload_texture2D();
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

/*
unsigned int 
texture_loadDDS(const char* path) {
	//https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673#file-load_dds-c

	// lay out variables to be used
	unsigned char* header;

	unsigned int width;
	unsigned int height;
	unsigned int mipMapCount;

	unsigned int blockSize;
	unsigned int format;

	unsigned int w;
	unsigned int h;

	unsigned char* buffer = 0;

	GLuint tid = 0;

	// open the DDS file for binary reading and get file size
	FILE* f;
	fopen_s(&f, path, "rb");
	
	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	// allocate new unsigned char space with 4 (file code) + 124 (header size) bytes
	// read in 128 bytes from the file
	header = (unsigned char*)malloc(128);
	fread(header, 1, 128, f);

	// compare the `DDS ` signature
	if (memcmp(header, "DDS ", 4) != 0) {
		free(buffer);
		free(header);
		fclose(f);
		std::cout << path << " failed loading (memcpy header failed)\n";
		return tid;
	}

	// extract height, width, and amount of mipmaps - yes it is stored height then width
	height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	
	// figure out what format to use for what fourCC file type it is
	// block size is about physical chunk storage of compressed data in file (important)
	if (header[84] == 'D') {
		switch (header[87]) {
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		case '0': // DX10
			// unsupported, else will error
			// as it adds sizeof(struct DDS_HEADER_DXT10) between pixels
			// so, buffer = malloc((file_size - 128) - sizeof(struct DDS_HEADER_DXT10));
			free(buffer);
			free(header);
			fclose(f);
			std::cout << path << " failed loading (DX10)\n";
			return tid;
			break;
		default: 
			free(buffer);
			free(header);
			fclose(f);
			std::cout << path << " failed loading (unknown format)\n";
			return tid;
		}
	}
	else // BC4U/BC4S/ATI2/BC55/R8G8_B8G8/G8R8_G8B8/UYVY-packed/YUY2-packed unsupported
	{
		free(buffer);
		free(header);
		fclose(f);
		std::cout << path << " failed loading (stragely packed)\n";
		return tid;
	}
	// allocate new unsigned char space with file_size - (file_code + header_size) magnitude
	// read rest of file
	buffer = (unsigned char*)malloc(file_size - 128);
	if (buffer == 0) {
		free(buffer);
		free(header);
		fclose(f);
		std::cout << path << " failed loading (unable to allocate buffer)\n";
		return tid;
	}

	fread(buffer, 1, file_size, f);
	for (int i = 0; i < file_size - 128; i++) {

	}

	// prepare new incomplete texture
	glGenTextures(1, &tid);
	if (tid == 0) {
		free(buffer);
		free(header);
		fclose(f);
		std::cout << path << " failed loading (unable to generate texture)\n";
		return tid;
	}

	// bind the texture
	// make it complete by specifying all needed parameters and ensuring all mipmaps are filled
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // opengl likes array length of mipmaps
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // don't forget to enable mipmaping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// prepare some variables
	unsigned int size = 0;
	w = width;
	h = height;
	//flip_image(buffer, w, h, blockSize);

	size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
	glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, size, buffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(buffer);
	free(header);
	fclose(f);
	return tid;
}
*/

void flip_image(unsigned char* bytes, int width, int height, int bytes_per_pixel) {

	unsigned char* buffer = (unsigned char*)malloc(bytes_per_pixel * width);

	for (int i = 0; i < height / 2; i++) {
		int offset = bytes_per_pixel * width * i;
		memcpy(buffer, bytes + offset, bytes_per_pixel * width);
		int offset2 = bytes_per_pixel * height * width;
		memcpy(bytes + offset, bytes + offset2 - offset, bytes_per_pixel * width);
		memcpy(bytes + offset2 - offset, buffer, bytes_per_pixel * width);		
	}
}
unsigned int
loadGLTexture(const std::string& filename, float gamma)
{
	GLint internalFormat = GL_RGBA;
	if (gamma == 2.2f) {
		internalFormat = GL_SRGB_ALPHA;
		gamma = 1.0f;
	}

	Image img;
	img.load(filename, gamma);

	unsigned int glTex;
	glGenTextures(1, &glTex);
	glBindTexture(GL_TEXTURE_2D, glTex);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, img.getWidth(), img.getHeight(), 0, GL_RGBA, GL_FLOAT, img.getPixels());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return glTex;
}

struct Edge
{
	glm::vec3 v[2];
	glm::uvec2 idx;
};

static inline bool
operator==(const Edge& e1, const Edge& e2)
{
	return (all(equal(e1.v[0], e2.v[0])) && all(equal(e1.v[1], e2.v[1])))
		|| (all(equal(e1.v[0], e2.v[1])) && all(equal(e1.v[1], e2.v[0])));
}

static inline size_t
hash_vec3(const glm::vec3& v)
{
	std::hash<float> hasher;
	size_t seed = hasher(v.x);
	seed ^= hasher(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= hasher(v.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	return seed;
}

static inline size_t
hash_edge(const Edge& e)
{
	return hash_vec3(e.v[0]) ^ hash_vec3(e.v[1]);
}

GLObjModel::
GLObjModel(const std::string& path, bool loadNormalMaps, bool loadTextures, int _flags)
	: flags(_flags)
{
	directory = "";
	for (int i = path.size() - 1; i >= 0; i--) {
		if (path.c_str()[i] == '/') {
			for (int j = 0; j <= i; j++) {
				directory += path.c_str()[j];
			}
			break;
		}
	}

	OBJFile obj_file(false);
	bool result = obj_file.loadFile(path);
	if (!result) {
		std::cerr << "could not load model \"" << path << "\"!" << std::endl;
		return;
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &buf_vertex);
	glGenBuffers(1, &buf_normal);
	glGenBuffers(1, &buf_texcoord);
	glGenBuffers(1, &buf_index);

	std::vector<glm::vec3> vertices, normals;
	std::vector<glm::vec2> tex_coords;
	std::vector<uint32_t> indices;

	auto hash_func = [](const glm::uvec3& v) -> size_t {
		std::hash<uint32_t> hasher;
		size_t seed = hasher(v.x);
		seed ^= hasher(v.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		seed ^= hasher(v.z) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		return seed;
	};

	std::unordered_map<glm::uvec3, uint32_t, decltype(hash_func)&> vertex_index_map(
		100000, hash_func
	);

	GLTextureList textures;

	for (unsigned i = 0; i < obj_file.getModelCount(); i++) {
		const auto& model = obj_file.getModel(i);
		vertex_index_map.clear();

		const auto& modelSurfaces = model->getSurfaces();
		for (const auto& surf : modelSurfaces) {
			GLObjModelSurface surfaceInfo;
			// save indices offset
			surfaceInfo.startIndex = indices.size();

			// get texture
			std::string dir = directory;
			auto get_texture = [&surf, &textures, &dir](const std::string& name) {
				const auto iter = surf->material->additionalInfo.find(name);
				if (iter != surf->material->additionalInfo.end())
				{
					return textures.getGLTexture(dir + iter->second);
				}
				return 0U;
			};
			if (loadTextures) {
				surfaceInfo.gl_texKa = get_texture("map_Ka");
				surfaceInfo.gl_texKd = get_texture("map_Kd");
				//if (!surfaceInfo.gl_texKd) surfaceInfo.gl_texKd = textures.getGLTexture("assets/empty.png");
				surfaceInfo.gl_texKs = get_texture("map_Ks");
				if (loadNormalMaps)
					surfaceInfo.gl_texBump = get_texture("map_Bump");
			}
			surfaceInfo.ka = surf->material->ambient;
			surfaceInfo.kd = surf->material->diffuse;
			surfaceInfo.ks = surf->material->specular;

			for (unsigned j = 0; j < surf->vertexIndices.size(); j++) {
				for (int k = 0; k < 3; k++) {
					glm::uvec3 idx(~0);
					idx[0] = surf->vertexIndices[j][k];
					if (surf->normalIndices.size() > 0) {
						idx[1] = surf->normalIndices.at(j)[k];
					}
					if (surf->texcoordIndices.size() > 0) {
						idx[2] = surf->texcoordIndices.at(j)[k];
					}

					uint32_t vertex_idx;
					auto it = vertex_index_map.find(idx);
					if (it != vertex_index_map.end()) {
						vertex_idx = it->second;
					}
					else {
						vertices.push_back(model->getVertices().at(idx[0]));
						if (surf->normalIndices.size() > 0) {
							normals.push_back(model->getNormals().at(idx[1]));
						}
						if (surf->texcoordIndices.size() > 0) {
							tex_coords.push_back(model->getTexcoords().at(idx[2]));
						}
						vertex_idx = vertices.size() - 1;
						vertex_index_map[idx] = vertex_idx;
					}
					indices.push_back(vertex_idx);
				}
			}

			// save end index and store to surface list
			surfaceInfo.endIndex = indices.size();
			modelSurfaceRange.push_back(surfaceInfo);
		}
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buf_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (normals.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, buf_normal);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (tex_coords.size() > 0) {
		glBindBuffer(GL_ARRAY_BUFFER, buf_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex_coords.size(), tex_coords.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}


	bool adjacency = flags & ADJACENCY;
	if (adjacency) {
		std::unordered_map<Edge, Edge, decltype(hash_edge)&> edge_table(100000, hash_edge);
		//std::unordered_map<glm::uvec2, glm::uvec2, decltype(hf)&> edge_table(100000, hf);

		//edge_table[glm::uvec2(1337, 42)] = glm::uvec2(10, 10);
		//cg_assert(edge_table.count(glm::uvec2(42, 1337)) > 0);

		for (size_t i = 0; i < indices.size(); i += 3) {
			glm::uvec3 idx = glm::uvec3(indices[i], indices[i + 1], indices[i + 2]);

			for (int j = 0; j < 3; j++) {
				//glm::uvec2 e = glm::uvec2(idx[j % 3], idx[(j + 1) % 3]);
				Edge e;
				e.v[0] = vertices.at(idx[(j + 0) % 3]);
				e.v[1] = vertices.at(idx[(j + 1) % 3]);
				e.idx = glm::uvec2(~0);

				//std::cout << e.v[0] << " " << e.v[1] << std::endl;

				if (edge_table.count(e) == 0) {
					edge_table[e] = e;
				}
				else {
					//std::cout << "found edge" << std::endl;
				}
				//std::cout << all(equal(edge_table[e].v[0], vertices[idx[j % 3]])) << std::endl;
				edge_table[e].idx[all(equal(edge_table[e].v[0], vertices[idx[j % 3]]))] = idx[(j + 2) % 3];
			}
		}

		std::vector<uint32_t> indices_adj;
		indices_adj.reserve(indices.size() * 3);

		for (size_t i = 0; i < indices.size(); i += 3) {
			glm::uvec3 idx = glm::uvec3(indices[i], indices[i + 1], indices[i + 2]);

			for (int j = 0; j < 3; j++) {
				//glm::uvec2 e = glm::uvec2(idx[j % 3], idx[(j + 1) % 3]);
				//glm::uvec2 ee = glm::uvec2(std::min(e.x, e.y), std::max(e.x, e.y));

				Edge e;
				e.v[0] = vertices[idx[(j + 0) % 3]];
				e.v[1] = vertices[idx[(j + 1) % 3]];

				//auto adj = edge_table[ee];


				indices_adj.push_back(idx[j % 3]);
				auto adj = edge_table[e].idx[!all(equal(edge_table[e].v[0], vertices[idx[j % 3]]))];
				// valid adjacency information found
				if (adj != ~0u) {
					indices_adj.push_back(adj);
				}
				else {
					indices_adj.push_back(idx[j % 3]);
				}
			}
		}

		indices = indices_adj;
	}



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
	

	std::cout << path << ": loaded " << vertices.size() << " vertices\n";
}

GLObjModel::
~GLObjModel()
{
	for (const GLObjModelSurface& faceData : modelSurfaceRange)
	{
		if (faceData.gl_texBump)
		{
			glDeleteTextures(1, &faceData.gl_texBump);
		}
		if (faceData.gl_texKs)
		{
			glDeleteTextures(1, &faceData.gl_texKs);
		}
		if (faceData.gl_texKd)
		{
			glDeleteTextures(1, &faceData.gl_texKd);
		}
		if (faceData.gl_texKa)
		{
			glDeleteTextures(1, &faceData.gl_texKa);
		}
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buf_vertex);
	glDeleteBuffers(1, &buf_normal);
	glDeleteBuffers(1, &buf_texcoord);
	glDeleteBuffers(1, &buf_index);
}

void GLObjModel::
draw(Shader* s, bool justbump)
{
	bool adjacency = flags & ADJACENCY;
	glBindVertexArray(vao);

	// draw each surface with texture
	for (const GLObjModelSurface& faceData : modelSurfaceRange)
	{
		if (s != nullptr) {
			if (justbump) {
				if (faceData.gl_texBump)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, faceData.gl_texBump);
					s->setInt("tex_bump", 0);
					s->setBool("useNormlaMap", true); 
					s->setBool("useTextures", true);
				}
				else {
					s->setBool("useNormlaMap", false); 
					s->setBool("useTextures", false);
				}
			}
			else {
				if (faceData.gl_texBump)
				{
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, faceData.gl_texBump);
					s->setInt("tex_bump", 3);
					s->setBool("useNormlaMap", true);
				}
				else {
					s->setBool("useNormlaMap", false);
				}
				if (faceData.gl_texKs)
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, faceData.gl_texKs);
					s->setInt("tex_k_s", 2);
				}
				if (faceData.gl_texKd)
				{
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, faceData.gl_texKd);
					s->setInt("tex_k_d", 1);
				}
				if (faceData.gl_texKa)
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, faceData.gl_texKa);
					s->setInt("tex_k_a", 0);
				}

				if (!faceData.gl_texBump && !faceData.gl_texKa && !faceData.gl_texKd && !faceData.gl_texKs)
				{
					s->setBool("useTextures", false);
				}
				else {
					s->setBool("useTextures", true);
				}

				s->setVec3("k_a", glm::value_ptr(faceData.ka));
				s->setVec3("k_d", glm::value_ptr(faceData.kd));
				s->setVec3("k_s", glm::value_ptr(faceData.ks));
			}
		}
		unsigned int numIndices = faceData.endIndex - faceData.startIndex;
		if (!adjacency) {
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (char*)0 + sizeof(unsigned int) * faceData.startIndex);
		}
		else {
			glDrawElements(GL_TRIANGLES_ADJACENCY, numIndices * 2, GL_UNSIGNED_INT, (char*)0 + sizeof(unsigned int) * faceData.startIndex * 2);
		}
	}

	glBindVertexArray(0);
}

void GLObjModel::
draw(int surface_range_idx)
{
	if (surface_range_idx >= modelSurfaceRange.size() || surface_range_idx < 0) return;

	bool adjacency = flags & ADJACENCY;
	glBindVertexArray(vao);

	auto& faceData = modelSurfaceRange[surface_range_idx];

	if (faceData.gl_texBump)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, faceData.gl_texBump);
	}
	if (faceData.gl_texKs)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, faceData.gl_texKs);
	}
	if (faceData.gl_texKd)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, faceData.gl_texKd);
	}
	if (faceData.gl_texKa)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, faceData.gl_texKa);
	}

	unsigned int numIndices = faceData.endIndex - faceData.startIndex;
	if (!adjacency) {
		glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, (char*)0 + sizeof(unsigned int) * faceData.startIndex);
	}
	else {
		glDrawElements(GL_TRIANGLES_ADJACENCY, numIndices * 2, GL_UNSIGNED_INT, (char*)0 + sizeof(unsigned int) * faceData.startIndex * 2);
	}

	glBindVertexArray(0);
}


int GLObjModel::
index_count() const
{
	int vc = 0;
	for (auto& i : modelSurfaceRange) {
		vc += (i.endIndex - i.startIndex);
	}
	return vc;
}
