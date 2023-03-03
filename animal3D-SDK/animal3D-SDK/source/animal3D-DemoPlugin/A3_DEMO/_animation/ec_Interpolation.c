#include "ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>
#include <Math.h>

#pragma region Helpers and defaults

void vtable_setDefaults(ec_DataVtable* out)
{
	out->lerp       = defaultLerp      ;
	out->nearest    = defaultNearest   ;
	out->cubic      = defaultCubic     ;
	out->deconcat   = defaultDeconcat  ;
	out->triangular = defaultTriangular;
	out->biLerp     = defaultBiLerp    ;
	out->biNearest  = defaultBiNearest ;
	out->biCubic    = defaultBiCubic   ;
}

void* defaultLerp(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, v0);            //val_out = v0
	funcs->invert(val_out);              //val_out = v0^-1
	funcs->concat(val_out, v1, val_out); //val_out = v1 * v0^-1
	funcs->scale(val_out, param);        //val_out = (v1 * v0^-1) * u
	funcs->concat(val_out, val_out, v0); //val_out = ((v1 * v0^-1) * u) * v0
	return val_out;
}

void* defaultTriangular(void* val_out, const void* v0, const void* v1, const void* v2, const a3real param1, const a3real param2, const ec_DataVtable* funcs)
{
	if (val_out && v0 && v1 && v2)
	{
		a3real param3 = 1 - param1 - param2;

		funcs->copy(val_out, v0);
		funcs->scale(val_out, param3);

		void* val1 = malloc(sizeof(v0));
		funcs->copy(val1, v1);
		funcs->scale(val1, param1);

		void* val2 = malloc(sizeof(v0));
		funcs->copy(val2, v2);
		funcs->scale(val2, param2);

		funcs->concat(val_out, val_out, val1);
		funcs->concat(val_out, val_out, val2);
	}
	
	return val_out;
}

void* defaultNearest(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	return funcs->lerp(val_out, v0, v1, param > 0.5f ? 1.0f : 0.0f, funcs);
}

void* defaultDeconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, rhs);            //val_out = rhs
	funcs->invert(val_out);               //val_out = rhs^-1
	funcs->concat(val_out, lhs, val_out); //val_out = lhs * rhs^-1
	return val_out;
}

void* defaultCubic(void* val_out, const void** vals, const a3real param, const ec_DataVtable* funcs)
{
	if (val_out && vals)
	{
		funcs->copy(val_out, vals[0]);

		void* val2 = malloc(sizeof(vals[0]));
		funcs->copy(val2, vals[1]);

		void* val3 = malloc(sizeof(vals[0]));
		funcs->copy(val3, vals[2]);

		void* val4 = malloc(sizeof(vals[0]));
		funcs->copy(val4, vals[3]);

		funcs->scale(val_out, (-param + 2 * pow(param, 2) - pow(param, 3)));
		funcs->scale(val2, (2 - 5 * pow(param, 2) + 3 * pow(param, 3)));
		funcs->scale(val3, (param + 4*pow(param, 2) - 3*pow(param, 3)));
		funcs->scale(val4, (-1 * pow(param, 2) + pow(param, 3)));

		funcs->concat(val_out, val_out, val2);
		funcs->concat(val_out, val_out, val3);
		funcs->concat(val_out, val_out, val4);

		funcs->scale(val_out, 0.5f);
	}
	return val_out;
}

void* defaultBiCubic(void* val_out, const void** vals[4], const a3real param0, const a3real param1, const ec_DataVtable* funcs)
{
	void* val1 = malloc(sizeof(vals[0]));
	funcs->copy(val1, vals[0]);

	void* val2 = malloc(sizeof(vals[0]));
	funcs->copy(val2, vals[1]);

	void* val3 = malloc(sizeof(vals[0]));
	funcs->copy(val3, vals[2]);

	void* val4 = malloc(sizeof(vals[0]));
	funcs->copy(val4, vals[3]);




	return val_out;
}

#pragma endregion

void setupVtables()
{
	
}
