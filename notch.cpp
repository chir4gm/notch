#include "common.h"
#include <filesystem>
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image.h>
glm::vec3 cam_pos = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 cam_front;
glm::vec3 cam_right;
float prevTime;
float yaw = -90.0f, pitch = 0.0f;
#define BLOCK_NONE 0
#define BLOCK_DIRT 1
#define BLOCK_STONE 2

#define BLOCK_SIZE 16
GLuint model_unif;
GLuint tex_idx_unif;
struct Chunk {
	uint8_t blocks[BLOCK_SIZE][BLOCK_SIZE][BLOCK_SIZE];
	glm::vec3 chunk_position;
	GLuint chunk_tex_id;
	Chunk(glm::vec3 pos) {
		chunk_position = pos;
		for (int x = 0; x < BLOCK_SIZE; x++) {
			for (int y = 0; y < BLOCK_SIZE; y++) {
				for (int z = 0; z < BLOCK_SIZE; z++) {
					blocks[x][y][z] = BLOCK_DIRT;
				}
			}
		}
		chunk_tex_id = 2;
	}
	void draw() {
		for (int x = 0; x < BLOCK_SIZE; x++) {
			for (int y = 0; y < BLOCK_SIZE; y++) {
				for (int z = 0; z < BLOCK_SIZE; z++) {
					if (blocks[x][y][z] != BLOCK_NONE) {
						glm::vec3 position = glm::vec3(x, y, z) + this->chunk_position;
						glm::mat4 model = glm::mat4(1.0f);
						model = glm::translate(model, position);
						glUniformMatrix4fv(model_unif, 1, GL_FALSE, glm::value_ptr(model));
						glUniform1ui(tex_idx_unif, blocks[x][y][z]);
						glDrawArrays(GL_TRIANGLES, 0, 36);
					}
				}
			}
		}
	}
};
struct ShaderProgram {
	GLuint id;
	GLuint get_id() {
		return id;
	}
	ShaderProgram(std::string vertex_path, std::string fragment_path) {
		id = glCreateProgram();
		GLuint vertex_shader, fragment_shader;
		
		vertex_shader = util::loadAndCompileShader(vertex_path, GL_VERTEX_SHADER);
		fragment_shader = util::loadAndCompileShader(fragment_path, GL_FRAGMENT_SHADER);

		if (vertex_shader != NULL and fragment_shader != NULL) {
			glAttachShader(id, vertex_shader);
			glAttachShader(id, fragment_shader);
			glLinkProgram(id);
			GLint linkSuccess;
			glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
			if (!linkSuccess) {
				GLint length;
				glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
				GLchar* infoLog = new GLchar[length];
				glGetProgramInfoLog(id, length, NULL, infoLog);
				std::cerr << "ERROR: program linking error: id:" << id << "\n";
				std::cerr << "log length:" << length << "\n";
				std::cerr << infoLog << "\n";
			}
		}
		else {
			id = NULL;
		}
	}
};
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_W) {
			cam_pos += cam_front;
		}
		if (key == GLFW_KEY_S) {
			cam_pos -= cam_front;
		}
		if (key == GLFW_KEY_A) {
			cam_pos -= cam_right;
		}
		if (key == GLFW_KEY_D) {
			cam_pos += cam_right;
		}

		if (key == GLFW_KEY_ESCAPE) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		if (key == GLFW_KEY_ENTER) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}
void mouse_enter_callback(GLFWwindow* window, int entered) {
	if (entered == GLFW_FALSE) {
		yaw = -90.0f;
		pitch = 0.0f;
	}
}
bool firstMouse = true;
double lastX, lastY;
void mouse_input_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	//    direction.y = 0.0f;
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam_front = glm::normalize(direction);
	cam_right = glm::cross(cam_front, glm::vec3(0.0f, 1.0f, 0.0f));
	cam_right = glm::normalize(cam_right);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
int main() {
	std::cout << "Current path is: " << std::filesystem::current_path() << std::endl;

	// start GL context and O/S window using the GLFW helper library
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	// uncomment these lines if on Apple OS X
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	GLFWwindow* window = glfwCreateWindow(480, 480, "Hello Triangle", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorEnterCallback(window, mouse_enter_callback);
	glfwSetCursorPosCallback(window, mouse_input_callback);
	glewExperimental = GL_TRUE;
	glewInit();

	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version:%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEPTH_TEST); // enable depth-testing

	float points[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), NULL);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// close GL context and any other GLFW resources

	int width, height, nChannels;
	uint8_t* blockTexture = stbi_load("../assets/terrain.png", &width, &height, &nChannels, STBI_rgb_alpha);
	unsigned int blockTextureID;
	if (!blockTexture) {
		fprintf(stdout, "New File Name\r\n");
		fprintf(stderr, "ERROR: Texture load failed\n");
		fprintf(stderr, stbi_failure_reason());
	}
	else {
		fprintf(stdout, "texture: w:%d h:%d c:%d", width, height, nChannels);
	}
	glGenTextures(1, &blockTextureID);
	glBindTexture(GL_TEXTURE_2D, blockTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)blockTexture);
	stbi_image_free((void*)blockTexture);

	ShaderProgram program("../shaders/basic.vert", "../shaders/basic.frag");
	if (program.get_id() == NULL) {
		return -1;
	}
	glUseProgram(program.get_id());

	glUniform1i(glGetUniformLocation(program.get_id(), "blockTexture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blockTextureID);



	GLuint view_unif = glGetUniformLocation(program.get_id(), "view");
	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(90.0f), 480.0f / 480.f, 0.1f, 100.0f);
	GLuint projection_unif = glGetUniformLocation(program.get_id(), "projection");

	model_unif = glGetUniformLocation(program.get_id(), "model");
	tex_idx_unif = glGetUniformLocation(program.get_id(), "tex_idx");

	prevTime = glfwGetTime();
	float prevSec = glfwGetTime();
	glm::vec3 cubePositions[] = {
	  glm::vec3(0.0f,  0.0f,  0.0f),
	  glm::vec3(2.0f,  5.0f, -15.0f),
	  glm::vec3(-1.5f, -2.2f, -2.5f),
	  glm::vec3(-3.8f, -2.0f, -12.3f),
	  glm::vec3(2.4f, -0.4f, -3.5f),
	  glm::vec3(-1.7f,  3.0f, -7.5f),
	  glm::vec3(1.3f, -2.0f, -2.5f),
	  glm::vec3(1.5f,  2.0f, -2.5f),
	  glm::vec3(1.5f,  0.2f, -1.5f),
	  glm::vec3(-1.3f,  1.0f, -1.5f)
	};
	Chunk chunk(glm::vec3(10.0, 0.0, 0.0));
	glBindVertexArray(vao);
	GLuint tex_idx = 0;
	while (!glfwWindowShouldClose(window)) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		float aspectRatio = (float)width / (float)height;
		glUniformMatrix4fv(projection_unif, 1, GL_FALSE, glm::value_ptr(projection));
		projection = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//		fprintf(stdout, "%f\n", prevTime);
		glm::mat4 view = glm::mat4(1.0f);

//		cam_pos.y = 0.0f;
		view = glm::lookAt(cam_pos, cam_pos + cam_front, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(view_unif, 1, GL_FALSE, glm::value_ptr(view));
		chunk.draw();
		if ((glfwGetTime() - prevSec) > 1.0f) {
			prevSec = glfwGetTime();
			tex_idx = (tex_idx == 200) ? 0 : tex_idx + 1;
		}
		prevTime = glfwGetTime();
		glfwSwapBuffers(window);

		glfwPollEvents();
	}
	glfwTerminate();

//	char* something_for_mem_leak = new char[23299];
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); 

	_CrtDumpMemoryLeaks();
	return 0;
}