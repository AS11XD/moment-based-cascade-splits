#include "TextRenderer.h"

TextRenderer::TextRenderer()
{
	s = new Shader("text.vert", "text.frag");

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("textures/alphabet.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "error loading image: alphabet\n";
	}
	stbi_image_free(data);
}

TextRenderer::~TextRenderer()
{
	s->~Shader();
	free(s);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteTextures(1, &tex);
}

float TextRenderer::getTextSizeX(std::string text, float sizeX)
{
	float thisX = 0.0f;
	int num = text.size() + 1;
	for (int i = 0; i < num; i++) {
		if (i != 0) {
			switch (text.c_str()[i - 1])
			{
			case '.':
			case ':':
				thisX += 0.4f * sizeX;
				break;

			case 'i':
				thisX += 0.6f * sizeX;
				break;

			case 'j':
			case 'l':
				thisX += 0.7f * sizeX;
				break;

			case 'I':
				thisX += 0.8f * sizeX;
				break;

			case 'f':
			case '%':
				thisX += 0.9f * sizeX;
				break;

			default:
				thisX += sizeX;
				break;
			}
		}
	}
	return thisX;
}

void TextRenderer::RenderText(std::string text, float x, float y, float sizeX, float sizeY, float r, float g, float b, float a, glm::mat4x4& MVP, bool center, float discardAtYBottom, float discardAtYTop)
{
	std::vector<float> vert;
	int num = text.size() + 1;
	float thisX = x;
	for (int i = 0; i < num; i++) {
		if (i != 0) {
			switch (text.c_str()[i - 1])
			{
			case '.':
			case ':':
				thisX += 0.4f * sizeX;
				break;

			case 'i':
				thisX += 0.6f * sizeX;
				break;

			case 'j':
			case 'l':
				thisX += 0.7f * sizeX;
				break;

			case 'I':
				thisX += 0.8f * sizeX;
				break;

			case 'f':
			case '%':
				thisX += 0.9f * sizeX;
				break;

			default:
				thisX += sizeX; 
				break;
			}
		} 

		vert.push_back(thisX);
		vert.push_back(y);
		vert.push_back(0.0f);

		vert.push_back(i);
		vert.push_back(1.0);
		
		vert.push_back(r);
		vert.push_back(g);
		vert.push_back(b);
		vert.push_back(a);

		vert.push_back(thisX);
		vert.push_back(y + sizeY);
		vert.push_back(0.0f);

		vert.push_back(i);
		vert.push_back(0.0);
		
		vert.push_back(r);
		vert.push_back(g);
		vert.push_back(b);
		vert.push_back(a);
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glDisable(GL_DEPTH_TEST);
	
	s->use();
	s->setMatrix4x4("MVP", glm::value_ptr(MVP));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	s->setInt("tex", 0);
	s->setInt("textlength", text.size());
	std::vector<int> textcharint;
	textcharint.resize(text.size());
	for (int i = 0; i < text.size(); i++)
		textcharint[i] = (int)text.c_str()[i];
	s->setIntVec("text", text.size(), textcharint.data());
	if (center) 
		s->setFloat("offsetX", (thisX - x) / 2.0);
	else 
		s->setFloat("offsetX", 0.0);
	s->setFloat("discardAtYBottom", discardAtYBottom);
	s->setFloat("discardAtYTop", discardAtYTop);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, vert.size() / 9);
	glEnable(GL_DEPTH_TEST);
}
