#include "A3_DEMO/_animation/ec_Integration.h"

#include <stdlib.h>

void* fIntegrateEuler(void* val_out, const void* x, const void* dx_dt, const a3real dt, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, dx_dt, funcs); // dx/dt
	funcs->scale(val_out, dt);          // (dx/dt)*t
	funcs->concat(val_out, x, val_out); // x + (dx/dt)*t
	return val_out;
}

void* fIntegrateKinematic(void* val_out, const void* x, const void* dx_dt, const void* d2x_dt2, const a3real dt, const ec_DataVtable* funcs)
{
	void* term3 = malloc(funcs->size);
	funcs->copy(term3, d2x_dt2, funcs); // d^2*x/dt^2
	funcs->scale(term3, dt*dt/2);       // (d^2*x/dt^2)*t^2 / 2 = term3

	funcs->copy(val_out, dx_dt, funcs); // dx/dt
	funcs->scale(val_out, dt);          // (dx/dt)*t = term2
	
	funcs->concat(val_out, val_out, term3); // term2 + term3
	funcs->concat(val_out, x, val_out);     // term1 + term2 + term3

	free(term3);
	return val_out;
}
