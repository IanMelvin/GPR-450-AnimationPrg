#include "A3_DEMO/_animation/ec_IK.h"

a3i32 ec_applyEffector_triangle(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy);

// ec_applyEffector impl. goes here

a3i32 ec_applyEffector_triangle(ec_IKEffector const* effector, a3_SpatialPose* poses_out, const a3_SpatialPose* posesRef_in, a3_Hierarchy const* hierarchy)
{
	//////// Solve positions ////////

	a3vec3 constraintDisplacement; //"c" in slides
	a3real3Diff(constraintDisplacement.v, effector->data.triangle.bendHint.v, posesRef_in[effector->workingRoot].translate.v);

	a3vec3 effectorDisplacement; //"d" (vector) in slides
	a3real3Diff(effectorDisplacement.v, effector->data.triangle.endTarget.v, posesRef_in[effector->workingRoot].translate.v);

	a3vec3 planeNormal; //"n_plane" in slides
	a3real3CrossUnit(planeNormal.v, effectorDisplacement.v, constraintDisplacement.v);

	a3vec3 h; //No idea what this actually means, but the slides say it's important
	a3real3CrossUnit(h.v, planeNormal.v, effectorDisplacement.v); //Technically slides say to normalize first, but this is more performant

	a3real L1 = a3real3Length(constraintDisplacement.v);
	a3vec3 L2v; a3real3Diff(L2v.v, effector->data.triangle.bendHint.v, effector->data.triangle.endTarget.v);
	a3real L2 = a3real3Length(L2v.v);
	a3real B = a3real3Length(effectorDisplacement.v);
	a3real s = ( B + L1 + L2 )/2;
	a3real A = a3sqrt( s*(s-B)*(s-L1)*(s-L2) );
	a3real H = 2*A/B;

	a3real D = a3sqrt( L1*L1 - H*H );

	a3vec3 Hh = h; a3real3MulS(&Hh.v, H);
	a3vec3 Dd = effectorDisplacement; a3real3MulS(&Dd.v, D);
	a3vec3* elbowPos_world = &poses_out[effector->data.triangle.elbowID]; //"p" in slides
	a3real3SetReal4(elbowPos_world->v, poses_out[effector->workingRoot].translate.v);
	a3real3Add(elbowPos_world->v, Dd.v);
	a3real3Add(elbowPos_world->v, Hh.v);

	// slide 50
}