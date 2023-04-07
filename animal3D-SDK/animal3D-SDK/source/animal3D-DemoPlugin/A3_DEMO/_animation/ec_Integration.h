/*
	Team Evistential Crisis
	Robert Christensen & Ian Melvin

	ec_Integration.h
	Calculus, but on unknown types
*/

#ifndef EC_INTEGRATION_H
#define EC_INTEGRATION_H

#include "ec_Interpolation.h"

void* fIntegrateEuler(void* val_out, const void* x, const void* dx_dt, const a3real dt, const ec_DataVtable* funcs);
void* fIntegrateKinematic(void* val_out, const void* x, const void* dx_dt, const void* d2x_dt2, const a3real dt, const ec_DataVtable* funcs);

//No need, would be the same as lerp
//void* fIntegrateInterpolated(void* val_out, const void* x, const void* xc, const a3real dt, const ec_DataVtable* funcs);

#endif