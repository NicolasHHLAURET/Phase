/*
 * PRNGObject.hpp
 *
 *  Created on: 9 oct. 2018
 *      Author: guilleuxj
 */

#ifndef INCLUDE_PRNGOBJECT_HPP_
#define INCLUDE_PRNGOBJECT_HPP_

#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Exit_if.h"
#include "Lib_malloc.h"
#include "GeneralDef.hpp"

#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define RNMX (1.0 - 1.2e-7)
#define MASK 123459876

#define EPSILON_ERROR 1e-5

/**
 * A graine initiale indentique, ce container genèrera la même suite de nombres.
 * C'est une simple encapsulation des générateurs pseudo aléatoire présents dans "lib_math.h".
 * Les statics sont remplacés par des variables de classe afin d'avoir plusieurs prng
 * en parrallèle pour le multithreading.
 */
class PRNGObject {
private:
	long idum;

	int iset;
	float gset;

	long iy;
	long iv[NTAB];
public:
	PRNGObject() {
		init(4269);
	}

	PRNGObject(long seed) {
		init(seed);
	}

	long getSeed() const {
		return idum;
	}

	void init(long seed) {
		iset = 0;
		gset = 0.0;
		iy = 0;
		idum = seed;
	}

	float prng_random1() {
		int j;
		long k;
		float temp;

		if (idum <= 0 || !iy) // Initialize
				{
			if (-(idum) < 1)
				idum = 1; // Be sure to prevent idum=0
			else
				idum = -(idum);

			for (j = NTAB + 7; j >= 0; j--) // Load the shuffle table
					{ //    after 8 warm-ups
				k = (idum) / IQ;
				idum = IA * (idum - k * IQ) - IR * k;
				if (idum < 0)
					idum += IM;
				if (j < NTAB)
					iv[j] = idum;
			}
			iy = iv[0];
		}
		k = (idum) / IQ; // Start here when not initializing
		idum = IA * (idum - k * IQ) - IR * k; // Compute idum=(IAidum)%IM
		if (idum < 0)
			idum += IM;
		j = (int) (iy / NDIV); // Will be in the range 0..NTAB-1
		iy = iv[j]; // Output previously stored value
		iv[j] = idum; //  and refill the shuffle table
		if ((temp = ((float) AM * iy)) > RNMX)
			return RNMX; // Because users don't except
		else
			return temp; // endpoint values
	}

	float prng_random0() {
		long k;
		float ans;

		idum ^= MASK;
		k = (idum) / IQ;
		idum = IA * (idum - k * IQ) - IR * k;
		if (idum < 0)
			idum += IM;
		//clf
		ans = ((float) AM * (idum));
		idum ^= MASK;
		return ans;
	}

	float prng_normal(	float m,
						float sigma) {
		if (iset == 0) {
			float fac, rsq, v1, v2;
			do {
				//clf
				v1 = 2.0f * prng_random0() - 1.0f;
				v2 = 2.0f * prng_random0() - 1.0f;
				rsq = v1 * v1 + v2 * v2;
			}
			//clf
			while (rsq >= 1.0 || fabs(rsq) < EPSILON_ERROR);
			fac = sqrtf(-2.0f * logf(rsq) / rsq);
			gset = v1 * fac;
			iset = 1;
			return v2 * fac * sigma + m;
		}
		iset = 0;
		return gset * sigma + m;
	}

};

#endif /* INCLUDE_PRNGOBJECT_HPP_ */
