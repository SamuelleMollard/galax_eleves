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
 
    std::fill(accelerationsx.begin(), accelerationsx.end(), 0);
    std::fill(accelerationsy.begin(), accelerationsy.end(), 0);
    std::fill(accelerationsz.begin(), accelerationsz.end(), 0);

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

	
	//OMP + MIPP version
	#pragma omp parallel
    {
		#pragma omp for schedule(static, 100)
		for (int i = 0; i < n_particles; i += mipp::N<float>()){
			// load registers body i
			mipp::Reg<float> rposx_i = &particles.x[i];
			mipp::Reg<float> rposy_i = &particles.y[i];
			mipp::Reg<float> rposz_i = &particles.z[i];
			mipp::Reg<float> raccx_i = &accelerationsx[i];
			mipp::Reg<float> raccy_i = &accelerationsy[i];
			mipp::Reg<float> raccz_i = &accelerationsz[i];

			for (int j = i+1; j < n_particles; j += mipp::N<float>()) {
				//load registers body j
				mipp::Reg<float> rposx_j = &particles.x[j];
				mipp::Reg<float> rposy_j = &particles.y[j];
				mipp::Reg<float> rposz_j = &particles.z[j];
				mipp::Reg<float> raccx_j = &accelerationsx[j];
				mipp::Reg<float> raccy_j = &accelerationsy[j];
				mipp::Reg<float> raccz_j = &accelerationsz[j];

				mipp::Reg<float> rdx = rposx_j - rposx_i;
				mipp::Reg<float> rdy = rposy_j - rposy_i;
				mipp::Reg<float> rdz = rposz_j - rposz_i;
				mipp::Reg<float> rdist = mipp::fmadd(rdx,rdx, mipp::fmadd(rdy,rdy,rdz * rdz));

				mipp::Reg<float> r10 = 10 ;
				//If dist < 1 then dist = 10
				// else dist = 10/sqrt(rdist^3)

				rdist = min(r10,r10/(mipp::rsqrt((rdist * rdist) * rdist)));	
				
				mipp::Reg<float> rnorm_factor_towards_i = rdist * initstate.masses[j];  // Distance * mass felt by particle i
				mipp::Reg<float> rnorm_factor_towards_j = rdist * initstate.masses[i];  // Distance * mass felt by particle j
				raccx_i += rdx * rnorm_factor_towards_i;
				raccy_i += rdy * rnorm_factor_towards_i;
				raccz_i += rdz * rnorm_factor_towards_i;
				raccx_j -= rdx * rnorm_factor_towards_j;  // Also compute the force i exerces onto j
				raccy_j -= rdy * rnorm_factor_towards_j;
				raccz_j -= rdz * rnorm_factor_towards_j;

				raccx_i.store(&accelerationsx[i]);
				raccy_i.store(&accelerationsy[i]);
				raccz_i.store(&accelerationsz[i]);
				raccx_j.store(&accelerationsx[j]);
				raccy_j.store(&accelerationsy[j]);
				raccz_j.store(&accelerationsz[j]);
			}
		}

		#pragma omp mutex
	    for (int i = 0; i < n_particles; i += mipp::N<float>()) {

			mipp::Reg<float> rvelx = &velocitiesx[i];
			mipp::Reg<float> rvely = &velocitiesx[i];
			mipp::Reg<float> rvelz = &velocitiesz[i];

			mipp::Reg<float> raccx = &accelerationsx[i];
			mipp::Reg<float> raccy = &accelerationsy[i];
			mipp::Reg<float> raccz = &accelerationsz[i];

			rvelx += raccx * 2.0f;
			rvely += raccy * 2.0f;
			rvelz += raccz * 2.0f;
	    }
	}

	for (int i = 0; i < n_particles; i += mipp::N<float>()){
			mipp::Reg<float> rvelx = &velocitiesx[i];
			mipp::Reg<float> rvely = &velocitiesx[i];
			mipp::Reg<float> rvelz = &velocitiesz[i];

			mipp::Reg<float> rposx = &particles.x[i];
			mipp::Reg<float> rposy = &particles.y[i];
			mipp::Reg<float> rposz = &particles.z[i];	
			
			rposx += rvelx  *0.1f;
			rposy += rvely  *0.1f;
			rposz += rvelz  *0.1f;

			rposx.store(&particles.x[i]);
			rposy.store(&particles.y[i]);
			rposz.store(&particles.z[i]);
	}
	

}

#endif // GALAX_MODEL_CPU_FAST
