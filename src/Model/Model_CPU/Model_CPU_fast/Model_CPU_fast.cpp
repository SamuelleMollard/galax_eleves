#ifdef GALAX_MODEL_CPU_FAST

#include <cmath>

#include "Model_CPU_fast.hpp"

#include <mipp.h>
#include <omp.h>

Model_CPU_fast::Model_CPU_fast(const Initstate& initstate, Particles& particles)
: Model_CPU(initstate, particles)
{
}

void Model_CPU_fast::step()
{
	/**
	#pragma omp parallel
	{
		std::vector<float> threadx(n_particles, 0);
		std::vector<float> thready(n_particles, 0);
		std::vector<float> threadz(n_particles, 0);
		#pragma omp for schedule(static, 100)
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
			   		threadx[i] += dx * norm_factor_towards_i;
			    	thready[i] += dy * norm_factor_towards_i;
			    	threadz[i] += dz * norm_factor_towards_i;
			    	threadx[j] -= dx * norm_factor_towards_j;  // Also compute the force i exerces onto j
			    	thready[j] -= dy * norm_factor_towards_j;
			    	threadz[j] -= dz * norm_factor_towards_j;
		    	}
	    	}
		#pragma omp mutex
		for (int i = 0; i < n_particles; ++i) {
			velocitiesx[i] += threadx[i] * 2.0f;
			velocitiesy[i] += thready[i] * 2.0f;
			velocitiesz[i] += threadz[i] * 2.0f;
		}
    }
	for (int i = 0; i < n_particles; ++i) {	
		particles.x[i] += velocitiesx[i] * 0.1f;
		particles.y[i] += velocitiesy[i] * 0.1f;
		particles.z[i] += velocitiesz[i] * 0.1f;
	}**/

	std::fill(accelerationsx.begin(), accelerationsx.end(), 0);
	std::fill(accelerationsy.begin(), accelerationsy.end(), 0);
	std::fill(accelerationsz.begin(), accelerationsz.end(), 0);

	//OMP + MIPP version
	#pragma omp parallel for
    
		
		for (int i = 0; i < n_particles; i ++){
			// load registers body i
			mipp::Reg<float> rposx_i = particles.x[i];
			mipp::Reg<float> rposy_i = particles.y[i];
			mipp::Reg<float> rposz_i = particles.z[i];
			mipp::Reg<float> raccx_i = accelerationsx[i];
			mipp::Reg<float> raccy_i = accelerationsy[i];
			mipp::Reg<float> raccz_i = accelerationsz[i];
			mipp::Reg<float> rmasses_i = initstate.masses[i];	

			for (int j = 0; j < n_particles; j += mipp::N<float>()) {
				if(i != j) {
				//load registers body j
				mipp::Reg<float> rposx_j = &particles.x[j];
				mipp::Reg<float> rposy_j = &particles.y[j];
				mipp::Reg<float> rposz_j = &particles.z[j];
				mipp::Reg<float> raccx_j = &accelerationsx[j];
				mipp::Reg<float> raccy_j = &accelerationsy[j];
				mipp::Reg<float> raccz_j = &accelerationsz[j];
				mipp::Reg<float> rmasses_j = &initstate.masses[j];
				
				mipp::Reg<float> rdx = rposx_j - rposx_i;
				mipp::Reg<float> rdy = rposy_j - rposy_i;
				mipp::Reg<float> rdz = rposz_j - rposz_i;
				mipp::Reg<float> rdist = rdx *rdx + rdy*rdy + rdz * rdz;

				mipp::Reg<float> r10 =10.0;
				rdist = min(r10,r10/(mipp::rsqrt(rdist * rdist * rdist)));
				
				accelerationsx[i] = hadd(rdx * rdist *rmasses_j);
				accelerationsy[i] = hadd(rdy * rdist *rmasses_j);
				accelerationsz[i] = hadd(rdz * rdist *rmasses_j);
				
				
				}
			}
		}

	    for (int i = 0; i < n_particles; i += mipp::N<float>()) {

			mipp::Reg<float> rvelx = &velocitiesx[i];
			mipp::Reg<float> rvely = &velocitiesy[i];
			mipp::Reg<float> rvelz = &velocitiesz[i];

			mipp::Reg<float> raccx = &accelerationsx[i];
			mipp::Reg<float> raccy = &accelerationsy[i];
			mipp::Reg<float> raccz = &accelerationsz[i];

			mipp::Reg<float> r2 = 2.0f;
			rvelx += raccx *r2;
			rvely += raccy * r2;
			rvelz += raccz * r2;
			
			mipp::Reg<float> rposx = &particles.x[i];
			mipp::Reg<float> rposy = &particles.y[i];
			mipp::Reg<float> rposz = &particles.z[i];	
			
			mipp::Reg<float> r01 = 0.1f;
			rposx += rvelx * r01;
			rposy += rvely * r01;
			rposz += rvelz * r01;

			rvelx.store(&velocitiesx[i]);
			rvely.store(&velocitiesy[i]);
			rvelz.store(&velocitiesz[i]);

			rposx.store(&particles.x[i]);
			rposy.store(&particles.y[i]);
			rposz.store(&particles.z[i]);
	    }

}

#endif // GALAX_MODEL_CPU_FAST
