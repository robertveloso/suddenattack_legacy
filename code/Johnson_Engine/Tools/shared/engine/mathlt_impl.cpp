
#include "bdefs.h"
#include "iltmath.h"
#include "geomroutines.h"



LTRESULT ILTMath::GetRotationVectors(LTRotation &rot, LTVector &right, LTVector &up, LTVector &forward)
{
	gr_GetRotationVectors(&rot, &right, &up, &forward);
	return LT_OK;
}


LTRESULT ILTMath::SetupEuler(LTRotation &rot, float pitch, float yaw, float roll)
{
	gr_EulerToRotation(pitch, yaw, roll, &rot);
	return LT_OK;
}


LTRESULT ILTMath::InterpolateRotation(LTRotation &rDest, LTRotation &rot1, LTRotation &rot2, float t)
{
	gr_InterpolateRotation(&rDest, &rot1, &rot2, t);
	return LT_OK;
}


LTRESULT ILTMath::SetupTransformationMatrix(LTMatrix &mMat, LTVector &vTranslation, LTRotation &rRot)
{
	gr_SetupTransformation(&vTranslation, &rRot, LTNULL, &mMat);
	return LT_OK;
}


LTRESULT ILTMath::SetupTranslationMatrix(LTMatrix &mMat, LTVector &vTranslation)
{
	gr_SetupTransformation(&vTranslation, LTNULL, LTNULL, &mMat);
	return LT_OK;
}


LTRESULT ILTMath::SetupRotationMatrix(LTMatrix &mMat, LTRotation &rRot)
{
	gr_SetupTransformation(LTNULL, &rRot, LTNULL, &mMat);
	return LT_OK;
}


LTRESULT ILTMath::SetupTranslationFromMatrix(LTVector &vTranslation, LTMatrix &mMat)
{
	mMat.GetTranslation(vTranslation);
	return LT_OK;
}


LTRESULT ILTMath::SetupRotationFromMatrix(LTRotation &rRot, LTMatrix &mMat)
{
	MatrixToRotation(&mMat, &rRot);
	return LT_OK;
}


LTRESULT ILTMath::SetupRotationAroundPoint(LTMatrix &mMat, LTRotation &rRot, LTVector &vPoint)
{
	LTMatrix mForward, mRotate, mBackward;
	LTVector negativePoint;

	negativePoint = -vPoint;
	SetupTranslationMatrix(mForward, vPoint);
	SetupTranslationMatrix(mBackward, negativePoint);
	SetupRotationMatrix(mRotate, rRot);

	mMat = mForward * mRotate * mBackward;
	return LT_OK;
}


LTRESULT ILTMath::AlignRotation(LTRotation &rOutRot, LTVector &vVector, LTVector &vUp)
{
	LTMatrix theMat;
	LTVector ref[3];

	// Get a frame of reference.
	gr_BuildFrameOfReference(&vVector, &vUp, &ref[0], &ref[1], &ref[2]);

	theMat.SetBasisVectors(&ref[0], &ref[1], &ref[2]);
	MatrixToRotation(&theMat, &rOutRot);
	return LT_OK;
}


LTRESULT ILTMath::EulerRotateX(LTRotation &rRot, float amount)
{
	LTVector vecs[3];

	gr_GetRotationVectors(&rRot, &vecs[0], &vecs[1], &vecs[2]);
	return RotateAroundAxis(rRot, vecs[0], amount);
}


LTRESULT ILTMath::EulerRotateY(LTRotation &rRot, float amount)
{
	LTVector vecs[3];

	gr_GetRotationVectors(&rRot, &vecs[0], &vecs[1], &vecs[2]);
	return RotateAroundAxis(rRot, vecs[1], amount);
}


LTRESULT ILTMath::EulerRotateZ(LTRotation &rRot, float amount)
{
	LTVector vecs[3];

	gr_GetRotationVectors(&rRot, &vecs[0], &vecs[1], &vecs[2]);
	return RotateAroundAxis(rRot, vecs[2], amount);
}


LTRESULT ILTMath::RotateAroundAxis(LTRotation &rRot, LTVector &vAxis, float amount)
{
	LTMatrix rotation, mat;

	// Get the quaternion.
	RotationToMatrix(&rRot, &mat);

	// Setup a rotation matrix and apply it.
	gr_SetupRotationAroundVector(&rotation, vAxis, amount);
	mat = rotation * mat;
	
	MatrixToRotation(&mat, &rRot);
	return LT_OK;
}


LTRESULT ILTMath::GetRotationVectorsFromMatrix(LTMatrix &mMat, 
		LTVector &vRight, LTVector &vUp, LTVector &vForward)
{
	mMat.GetBasisVectors(&vRight, &vUp, &vForward);
	return LT_OK;
}






