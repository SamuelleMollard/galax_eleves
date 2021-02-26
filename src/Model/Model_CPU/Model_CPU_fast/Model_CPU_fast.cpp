#ifdef GALAX_MODEL_CPU_FAST

#include <cmath>

#include "Model_CPU_fast.hpp"

#include <mipp.h>
#include <omp.h>

Model_CPU_fast
::Model_CPU_fast(const Initstate& initstate, Particles& particles)
: Model_CPU(initstate, particles)
{
}

void Model_CPU_fast
::step()
{
    std::fill(accelerationsx.begin(), accelerationsx.end(), 0);
    std::fill(accelerationsy.begin(), accelerationsy.end(), 0);
    std::fill(accelerationsz.begin(), accelerationsz.end(), 0);

	#pragma omp parallel for schedule(static,100)
	for (int i = 0; i < n_particles; ++i) {
		for (int j = i+1; j < n_particles; ++j) {
			float dx = particles.x[j] - particles.x[i];
			float dy = particles.y[j] - particles.y[i];
			float dz = particles.z[j] - particles.z[i];
			float dist = dx * dx + dy * dy + dz * dz;
			
			if (dist < 1.0) {
				dist = 10;
			}
			else {
				dist = 10 / std::sqrt(dist * dist * dist);
			}

			float norm_factor_towards_i = dist * initstate.masses[j];  // Distance * mass felt by particle i
			float norm_factor_towards_j = dist * initstate.masses[i];  // Distance * mass felt by particle j
			accelerationsx[i] += dx * norm_factor_towards_i;
			accelerationsy[i] += dy * norm_factor_towards_i;
			accelerationsz[i] += dz * norm_factor_towards_i;
			accelerationsx[j] -= dx * norm_factor_towards_j;  // Also compute the force i exerces onto j
			accelerationsy[j] -= dy * norm_factor_towards_j;
			accelerationsz[j] -= dz * norm_factor_towards_j;
		}
	}
	for (int i = 0; i < n_particles; ++i) {	
		velocitiesx[i] += accelerationsx[i] * 2.0f;
		velocitiesy[i] += accelerationsy[i] * 2.0f;
		velocitiesz[i] += accelerationsz[i] * 2.0f;
		particles.x[i] += velocitiesx[i] * 0.1f;
		particles.y[i] += velocitiesy[i] * 0.1f;
		particles.z[i] += velocitiesz[i] * 0.1f;
	}
// OMP  version
// #pragma omp parallel for
//     for (int i = 0; i < n_particles; i += mipp::N<float>())
//     {
//     }


// OMP + MIPP version
// #pragma omp parallel for
//     for (int i = 0; i < n_particles; i += mipp::N<float>())
//     {
//         // load registers body i
//         const mipp::Reg<float> rposx_i = &particles.x[i];
//         const mipp::Reg<float> rposy_i = &particles.y[i];
//         const mipp::Reg<float> rposz_i = &particles.z[i];
//               mipp::Reg<float> raccx_i = &accelerationsx[i];
//               mipp::Reg<float> raccy_i = &accelerationsy[i];
//               mipp::Reg<float> raccz_i = &accelerationsz[i];
//     }
}

#endif // GALAX_MODEL_CPU_FAST
