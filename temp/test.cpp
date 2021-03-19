#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <string>
#include <fstream>
#include <GL/glut.h>

bool random_bool() {
	return rand() > (RAND_MAX / 2);
}

std::string readFile(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    return content;
}

void gpu_compute() {

	auto gComputeProgram = glCreateProgram();
	GLuint mComputeShader = glCreateShader(GL_COMPUTE_SHADER);

	const char *shader_file = readFile("compute.glsl").c_str();

	glShaderSource(mComputeShader, 1, &shader_file, NULL);
	glCompileShader(mComputeShader);

	int rvalue;
	int length;
	glGetShaderiv(mComputeShader, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		GLint maxLength = 0;
		glGetShaderiv(mComputeShader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(mComputeShader, maxLength, &maxLength, &errorLog[0]);
		for (int i = 0; i < errorLog.size(); ++i) {
			std::cerr << errorLog[i];
		}
		std::cerr << std::endl;
		return ;
	}

	// Attach and link the shader against to the compute program
	glAttachShader(gComputeProgram, mComputeShader);
	glLinkProgram(gComputeProgram);

	// Check if there were some issues when linking the shader.
	glGetProgramiv(gComputeProgram, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
	       printf("Error get program iv : %d", rvalue);
	       return ;
	}
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutCreateWindow("Just to load glew");
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error initialising glew: " << glewGetErrorString(err) << std::endl;
	}
	else {
		std::cerr << "Loaded glew" << std::endl;
	}
	std::vector<bool> tab(10000000, false);
	for (int i = 0; i < tab.size(); ++i) {
		tab[i] = random_bool();
	}
	
	auto t1 = std::chrono::high_resolution_clock::now();
	
	int sum = 0;
	for (int i = 0; i < tab.size(); ++i) {
		if (tab[i]) {
			sum ++;
		}
	}

	auto t2 = std::chrono::high_resolution_clock::now();

	gpu_compute();

	auto t3 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> duration_s = t2 - t1;
	std::cout << "Duration for CPU: " << duration_s.count() << std::endl;
	std::cout << "CPU result : " << sum << std::endl;

	//duration_s = t3 - t2;
	//std::cout << "Duration for GPU: " << duration_s.count() << std::endl;

}
