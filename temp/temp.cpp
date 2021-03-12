#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

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

int main(int argc, char ** argv) {
  /* ----- Compute Shader ----- */

  printWorkGroupsCapabilities();

  GLuint computeShaderID;
  GLuint csProgramID;
  char * computeShader = 0;

  GLint Result = GL_FALSE;
  int InfoLogLength = 1024;
  char ProgramErrorMessage[1024] = { 0 };

  computeShaderID = glCreateShader(GL_COMPUTE_SHADER);

  loadShader(&computeShader, "compute.shader");
  compileShader(computeShaderID, computeShader);

  csProgramID = glCreateProgram();

  glAttachShader(csProgramID, computeShaderID);
  glLinkProgram(csProgramID);
  glDeleteShader(computeShaderID);

  /* ----- Run Compute shader ----- */
  glUseProgram(csProgramID);
  glBindTexture(GL_TEXTURE_2D, quadTextureID);
  glBindImageTexture(0, quadTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glDispatchCompute(40, 30, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
  glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUseProgram(0);

  return 0;
}

