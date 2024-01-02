#include "common.h"
#include "util.h"

namespace util {
	GLuint loadAndCompileShader(std::string path, GLenum type) {
		std::ifstream shader_file(path);
		std::stringstream shader_buffer;
		std::string shader_source_string;
		if (!shader_file) {
			std::cerr << "ERROR:" << path << ": file not found" << std::endl;
		}
		GLuint shader;
		shader = glCreateShader(type);
		shader_buffer << shader_file.rdbuf();
		shader_source_string = shader_buffer.str();
		const char* shader_source_ptr = shader_source_string.c_str();
		glShaderSource(shader, 1, &shader_source_ptr, NULL);
		glCompileShader(shader);
		GLint success;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			GLint length;
			GLchar* infoLog = new GLchar[length];
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			glGetShaderInfoLog(shader, length, NULL, infoLog);
			std::cerr << "ERROR: shader compile error\n" << path << "\n";
			std::cerr << infoLog << "\n";
			delete[]infoLog;
			glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);
			infoLog = new GLchar[length];
			glGetShaderSource(shader, length, NULL, infoLog);
			std::cerr << "source:\n";
			std::cerr << infoLog << "\n";
			return NULL;
		}
		return shader;
	}

}