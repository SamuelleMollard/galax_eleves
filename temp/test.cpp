#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <random>
#include <vector>
#include <chrono>
#include <GL/glew.h>

bool random_bool() {
	return rand() > (RAND_MAX / 2);
}

void printWorkGroupsCapabilities() {
  int workgroup_count[3];
  int workgroup_size[3];
  int workgroup_invocations;

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workgroup_count[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workgroup_count[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workgroup_count[2]);

  printf ("Taille maximale des workgroups:\n\tx:%u\n\ty:%u\n\tz:%u\n",
  workgroup_size[0], workgroup_size[1], workgroup_size[2]);

  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workgroup_size[0]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workgroup_size[1]);
  glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workgroup_size[2]);

  printf ("Nombre maximal d'invocation locale:\n\tx:%u\n\ty:%u\n\tz:%u\n",
  workgroup_size[0], workgroup_size[1], workgroup_size[2]);

  glGetIntegerv (GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workgroup_invocations);
  printf ("Nombre maximum d'invocation de workgroups:\n\t%u\n", workgroup_invocations);
}


int main() {
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
	std::chrono::duration<float> duration_s = t2 - t1;
	std::cout << "Duration for CPU: " << duration_s.count() << std::endl;
	std::cout << "CPU result : " << sum << std::endl;

	printWorkGroupsCapabilities();
}
