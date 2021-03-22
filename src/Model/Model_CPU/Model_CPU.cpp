#include <cmath>

#include "Model_CPU.hpp"

Model_CPU
::Model_CPU(const Initstate& initstate, Particles& particles)
: Model(initstate, particles),
  velocitiesx   (n_particles+3),
  velocitiesy   (n_particles+3),
  velocitiesz   (n_particles+3),
  accelerationsx(n_particles+3),
  accelerationsy(n_particles+3),
  accelerationsz(n_particles+3)
{
	for (int i = 0; i < n_particles; i++)
	{
		particles.x[i] = initstate.positionsx[i];
		particles.y[i] = initstate.positionsy[i];
		particles.z[i] = initstate.positionsz[i];
	}

      
    std::copy(initstate.velocitiesx.begin(), initstate.velocitiesx.end(), velocitiesx.begin());
    std::copy(initstate.velocitiesy.begin(), initstate.velocitiesy.end(), velocitiesy.begin());
    std::copy(initstate.velocitiesz.begin(), initstate.velocitiesz.end(), velocitiesz.begin());

    for (int i = 0; i < 3; i++)
	{
		particles.x.push_back(0);
		particles.y.push_back(0);
        particles.z.push_back(0);
        velocitiesx[n_particles+i]=0;
        velocitiesy[n_particles+i]=0;
        velocitiesz[n_particles+i]=0;
	} 
}
