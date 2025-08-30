#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

static std::string ReadFile(const std::string& path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file)
		throw std::runtime_error("failed to read file at: " + path);

	std::ostringstream content;
	content << file.rdbuf();
	return content.str();
}

static unsigned int CompileShader(GLenum type, const std::string& src) {
	GLuint id = glCreateShader(type);
	const char* csrc = src.c_str();
	glShaderSource(id, 1, &csrc, nullptr);
	glCompileShader(id);
	GLint ok = GL_FALSE; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if (!ok) { GLint len=0; glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		std::string log(len, '\0'); glGetShaderInfoLog(id, len, &len, log.data());
		std::cerr << "Shader compile error:\n" << log << std::endl;
		glDeleteShader(id); return 0; }
	return id;
}

static unsigned int CreateProgram(const std::string& vs, const std::string& fs) {
	GLuint prog = glCreateProgram();
	GLuint v = CompileShader(GL_VERTEX_SHADER, vs);
	GLuint f = CompileShader(GL_FRAGMENT_SHADER, fs);
	glAttachShader(prog, v); glAttachShader(prog, f);
	glLinkProgram(prog);
	GLint ok = GL_FALSE; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
	if (!ok) { GLint len=0; glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
		std::string log(len, '\0'); glGetProgramInfoLog(prog, len, &len, log.data());
		std::cerr << "Program link error:\n" << log << std::endl;
	}
	glDeleteShader(v); glDeleteShader(f);
	return prog;
}

int main()
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// load GLAD for modern opengl
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	// --- GPU resources ---
	unsigned int vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	std::string vs = ReadFile(std::string(PROJECT_ROOT) + "/res/shaders/shader.vert");
	std::string fs = ReadFile(std::string(PROJECT_ROOT) + "/res/shaders/shader.frag");
	unsigned int program = CreateProgram(vs, fs);
	glUseProgram(program);

	glClearColor(0.07f, 0.07f, 0.1f, 1.0f);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glfwTerminate();
	return 0;
}
