#include "ec_Interpolation.h"

#include <stdlib.h>
#include <string.h>

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
	funcs->invert(val_out);              //val_out = Invert(v0)
	funcs->concat(val_out, v1, val_out); //val_out = v1 Concat Invert(v0)
	funcs->scale(val_out, param);        //val_out = (v1 Concat Invert(v0)) Scale u
	funcs->concat(val_out, val_out, v0); //val_out = ((v1 Concat Invert(v0)) Scale u) Concat v0
	return val_out;
}

void* defaultDeconcat(void* val_out, const void* lhs, const void* rhs, const ec_DataVtable* funcs)
{
	funcs->copy(val_out, rhs);
	funcs->invert(val_out);
	funcs->concat(val_out, lhs, val_out);
	return val_out;
}

void* defaultNearest(void* val_out, const void* v0, const void* v1, const a3real param, const ec_DataVtable* funcs)
{
	return funcs->lerp(val_out, v0, v1, param>0.5f ? 1.0f : 0.0f, funcs);
}

#pragma endregion

void setupVtables()
{
	
}
