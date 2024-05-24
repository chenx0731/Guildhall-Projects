#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"

Mat44::Mat44()
{
	m_value[Ix] = 1.f;
	m_value[Iy] = 0.f;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = 0.f;
	m_value[Jy] = 1.f;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Kx] = 0.f;
	m_value[Ky] = 0.f;
	m_value[Kz] = 1.f;
	m_value[Kw] = 0.f;

	m_value[Tx] = 0.f;
	m_value[Ty] = 0.f;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;
}

Mat44::Mat44(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D)
{
	m_value[Ix] = 1.f;
	m_value[Iy] = 0.f;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = 0.f;
	m_value[Jy] = 1.f;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Kx] = 0.f;
	m_value[Ky] = 0.f;
	m_value[Kz] = 1.f;
	m_value[Kw] = 0.f;

	m_value[Tx] = 0.f;
	m_value[Ty] = 0.f;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;

	m_value[Ix] = iBasis2D.x;
	m_value[Iy] = iBasis2D.y;
	
	m_value[Jx] = jBasis2D.x;
	m_value[Jy] = jBasis2D.y;

	m_value[Tx] = translation2D.x;
	m_value[Ty] = translation2D.y;
}

Mat44::Mat44(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D)
{
	m_value[Ix] = 1.f;
	m_value[Iy] = 0.f;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = 0.f;
	m_value[Jy] = 1.f;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Kx] = 0.f;
	m_value[Ky] = 0.f;
	m_value[Kz] = 1.f;
	m_value[Kw] = 0.f;

	m_value[Tx] = 0.f;
	m_value[Ty] = 0.f;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;

	m_value[Ix] = iBasis3D.x;
	m_value[Iy] = iBasis3D.y;
	m_value[Iz] = iBasis3D.z;
						
	m_value[Jx] = jBasis3D.x;
	m_value[Jy] = jBasis3D.y;
	m_value[Jz] = jBasis3D.z;

	m_value[Kx] = kBasis3D.x;
	m_value[Ky] = kBasis3D.y;
	m_value[Kz] = kBasis3D.z;

	m_value[Tx] = translation3D.x;
	m_value[Ty] = translation3D.y;
	m_value[Tz] = translation3D.z;
}

Mat44::Mat44(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_value[Ix] = iBasis4D.x;
	m_value[Iy] = iBasis4D.y;
	m_value[Iz] = iBasis4D.z;
	m_value[Iw] = iBasis4D.w;
						
	m_value[Jx] = jBasis4D.x;
	m_value[Jy] = jBasis4D.y;
	m_value[Jz] = jBasis4D.z;
	m_value[Jw] = jBasis4D.w;

	m_value[Kx] = kBasis4D.x;
	m_value[Ky] = kBasis4D.y;
	m_value[Kz] = kBasis4D.z;
	m_value[Kw] = kBasis4D.w;

	m_value[Tx] = translation4D.x;
	m_value[Ty] = translation4D.y;
	m_value[Tz] = translation4D.z;
	m_value[Tw] = translation4D.w;
}

Mat44::Mat44(float const* sixteenValuesBasisMajor)
{
	m_value[Ix] = sixteenValuesBasisMajor[0];
	m_value[Iy] = sixteenValuesBasisMajor[1];
	m_value[Iz] = sixteenValuesBasisMajor[2];
	m_value[Iw] = sixteenValuesBasisMajor[3];
				
	m_value[Jx] = sixteenValuesBasisMajor[4];
	m_value[Jy] = sixteenValuesBasisMajor[5];
	m_value[Jz] = sixteenValuesBasisMajor[6];
	m_value[Jw] = sixteenValuesBasisMajor[7];
		
	m_value[Kx] = sixteenValuesBasisMajor[8];
	m_value[Ky] = sixteenValuesBasisMajor[9];
	m_value[Kz] = sixteenValuesBasisMajor[10];
	m_value[Kw] = sixteenValuesBasisMajor[11];
				  
	m_value[Tx] = sixteenValuesBasisMajor[12];
	m_value[Ty] = sixteenValuesBasisMajor[13];
	m_value[Tz] = sixteenValuesBasisMajor[14];
	m_value[Tw] = sixteenValuesBasisMajor[15];
}

Mat44 const Mat44::CreateTranslation2D(Vec2 const& translationXY)
{
	Mat44 mat44;
	mat44.m_value[Tx] = translationXY.x;
	mat44.m_value[Ty] = translationXY.y;
	return mat44;
}

Mat44 const Mat44::CreateTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 mat44;
	mat44.m_value[Tx] = translationXYZ.x;
	mat44.m_value[Ty] = translationXYZ.y;
	mat44.m_value[Tz] = translationXYZ.z;
	return mat44;
}

Mat44 const Mat44::CreateUniformScale2D(float uniformScaleXY)
{
	Mat44 mat44;
	mat44.m_value[Ix] = uniformScaleXY;
	mat44.m_value[Jy] = uniformScaleXY;
	return mat44;
}

Mat44 const Mat44::CreateUniformScale3D(float uniformScaleXYZ)
{
	Mat44 mat44;
	mat44.m_value[Ix] = uniformScaleXYZ;
	mat44.m_value[Jy] = uniformScaleXYZ;
	mat44.m_value[Kz] = uniformScaleXYZ;
	return mat44;
}

Mat44 const Mat44::CreateNonUniformScale2D(Vec2 const& nonUniformScaleXY)
{
	Mat44 mat;
	mat.m_value[Ix] = nonUniformScaleXY.x;
	mat.m_value[Jy] = nonUniformScaleXY.y;
	return mat;
}

Mat44 const Mat44::CreateNonUniformScale3D(Vec3 const& nonUniformScaleXYZ)
{
	Mat44 mat;
	mat.m_value[Ix] = nonUniformScaleXYZ.x;
	mat.m_value[Jy] = nonUniformScaleXYZ.y;
	mat.m_value[Kz] = nonUniformScaleXYZ.z;
	return mat;
}

Mat44 const Mat44::CreateZRotationDegrees(float rotationDegreesAboutZ)
{
	Mat44 mat44;
	mat44.m_value[Ix] = CosDegrees(rotationDegreesAboutZ);
	mat44.m_value[Jy] = CosDegrees(rotationDegreesAboutZ);
	mat44.m_value[Iy] = SinDegrees(rotationDegreesAboutZ);
	mat44.m_value[Jx] = -SinDegrees(rotationDegreesAboutZ);
	return mat44;
}

Mat44 const Mat44::CreateYRotationDegrees(float rotationDegreesAboutY)
{
	Mat44 mat44;
	mat44.m_value[Ix] = CosDegrees(rotationDegreesAboutY);
	mat44.m_value[Kz] = CosDegrees(rotationDegreesAboutY);
	mat44.m_value[Iz] = -SinDegrees(rotationDegreesAboutY);
	mat44.m_value[Kx] = SinDegrees(rotationDegreesAboutY);
	return mat44;
}

Mat44 const Mat44::CreateXRotationDegrees(float rotationDegreesAboutX)
{
	Mat44 mat44;
	mat44.m_value[Jy] = CosDegrees(rotationDegreesAboutX);
	mat44.m_value[Kz] = CosDegrees(rotationDegreesAboutX);
	mat44.m_value[Jz] = SinDegrees(rotationDegreesAboutX);
	mat44.m_value[Ky] = -SinDegrees(rotationDegreesAboutX);
	return mat44;
}

Mat44 const Mat44::CreateOrthoProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	Mat44 mat44;
	mat44.m_value[Ix] = 2.f / (right - left);
	mat44.m_value[Jy] = 2.f / (top - bottom);
	mat44.m_value[Kz] = 1.f / (zFar - zNear);
	mat44.m_value[Tx] = (left + right) / (left - right);
	mat44.m_value[Ty] = (bottom + top) / (bottom - top);
	mat44.m_value[Tz] = zNear / (zNear - zFar);
	return mat44;
}

Mat44 const Mat44::CreatePerspectiveProjection(float fovYDegrees, float aspect, float zNear, float zFar)
{
	Mat44 mat44;
	float c = CosDegrees(fovYDegrees * 0.5f);
	float s = SinDegrees(fovYDegrees * 0.5f);
	float scaleY = c / s;
	float scaleX = scaleY / aspect;
	float scaleZ = zFar / (zFar - zNear);
	float translateZ = (zNear * zFar) / (zNear - zFar);

	mat44.m_value[Ix] = scaleX;
	mat44.m_value[Jy] = scaleY;
	mat44.m_value[Kz] = scaleZ;
	mat44.m_value[Kw] = 1.f;
	mat44.m_value[Tz] = translateZ;
	mat44.m_value[Tw] = 0.f;

	return mat44;
}

Vec2 const Mat44::TransformVectorQuantity2D(Vec2 const& vectorQuantityXY) const
{
	float x = DotProduct4D(Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]), Vec4(vectorQuantityXY));
	float y = DotProduct4D(Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]), Vec4(vectorQuantityXY));
	return Vec2(x, y);
}

Vec3 const Mat44::TransformVectorQuantity3D(Vec3 const& vectorQuantityXYZ) const
{
	float x = DotProduct4D(Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]), Vec4(vectorQuantityXYZ));
	float y = DotProduct4D(Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]), Vec4(vectorQuantityXYZ));
	float z = DotProduct4D(Vec4(m_value[Iz], m_value[Jz], m_value[Kz], m_value[Tz]), Vec4(vectorQuantityXYZ));
	return Vec3(x, y, z);
}

Vec2 const Mat44::TransformPosition2D(Vec2 const& positionXY) const
{
	float x = DotProduct4D(Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]), Vec4(positionXY.x, positionXY.y, 0.f, 1.f));
	float y = DotProduct4D(Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]), Vec4(positionXY.x, positionXY.y, 0.f, 1.f));
	return Vec2(x, y);
}

Vec3 const Mat44::TransformPosition3D(Vec3 const& positionXYZ) const
{
	float x = DotProduct4D(Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));
	float y = DotProduct4D(Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));
	float z = DotProduct4D(Vec4(m_value[Iz], m_value[Jz], m_value[Kz], m_value[Tz]), Vec4(positionXYZ.x, positionXYZ.y, positionXYZ.z, 1.f));
	return Vec3(x, y, z);
}

Vec4 const Mat44::TransformHomogeneous3D(Vec4 const& homogeneousPoint3D) const
{
	float x = DotProduct4D(Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]), Vec4(homogeneousPoint3D));
	float y = DotProduct4D(Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]), Vec4(homogeneousPoint3D));
	float z = DotProduct4D(Vec4(m_value[Iz], m_value[Jz], m_value[Kz], m_value[Tz]), Vec4(homogeneousPoint3D));
	float w = DotProduct4D(Vec4(m_value[Iw], m_value[Jw], m_value[Kw], m_value[Tw]), Vec4(homogeneousPoint3D));
	return Vec4(x, y, z, w);
}

float* Mat44::GetAsFloatArray()
{
	return &m_value[0];
}

float const* Mat44::GetAsFloatArray() const
{
	return &m_value[0];
}

Vec2 const Mat44::GetIBasis2D() const
{
	return Vec2(m_value[Ix], m_value[Iy]);
}

Vec2 const Mat44::GetJBasis2D() const
{
	return Vec2(m_value[Jx], m_value[Jy]);
}

Vec2 const Mat44::GetTranslation2D() const
{
	return Vec2(m_value[Tx], m_value[Ty]);
}

Vec3 const Mat44::GetIBasis3D() const
{
	return Vec3(m_value[Ix], m_value[Iy], m_value[Iz]);
}

Vec3 const Mat44::GetJBasis3D() const
{
	return Vec3(m_value[Jx], m_value[Jy], m_value[Jz]);
}

Vec3 const Mat44::GetKBasis3D() const
{
	return Vec3(m_value[Kx], m_value[Ky], m_value[Kz]);
}

Vec3 const Mat44::GetTranslation3D() const
{
	return Vec3(m_value[Tx], m_value[Ty], m_value[Tz]);
}

Vec4 const Mat44::GetIBasis4D() const
{
	return Vec4(m_value[Ix], m_value[Iy], m_value[Iz], m_value[Iw]);
}

Vec4 const Mat44::GetJBasis4D() const
{
	return Vec4(m_value[Jx], m_value[Jy], m_value[Jz], m_value[Jw]);
}

Vec4 const Mat44::GetKBasis4D() const
{
	return Vec4(m_value[Kx], m_value[Ky], m_value[Kz], m_value[Kw]);
}

Vec4 const Mat44::GetTranslation4D() const
{
	return Vec4(m_value[Tx], m_value[Ty], m_value[Tz], m_value[Tw]);
}

Mat44 const Mat44::GetOrthonormalInverse() const
{
	Mat44 mat1, mat2;
	mat1.m_value[Ix] = m_value[Ix];
	mat1.m_value[Jy] = m_value[Jy];
	mat1.m_value[Kz] = m_value[Kz];
	mat1.m_value[Iy] = m_value[Jx];
	mat1.m_value[Iz] = m_value[Kx];
	mat1.m_value[Jz] = m_value[Ky];
	mat1.m_value[Jx] = m_value[Iy];
	mat1.m_value[Kx] = m_value[Iz];
	mat1.m_value[Ky] = m_value[Jz];
	mat2.m_value[Tx] = -m_value[Tx];
	mat2.m_value[Ty] = -m_value[Ty];
	mat2.m_value[Tz] = -m_value[Tz];
	mat1.Append(mat2);
	return mat1;
}


Vec4 const Mat44::GetXDimension4D() const
{
	return Vec4(m_value[Ix], m_value[Jx], m_value[Kx], m_value[Tx]);
}

Vec4 const Mat44::GetYDimension4D() const
{
	return Vec4(m_value[Iy], m_value[Jy], m_value[Ky], m_value[Ty]);
}

Vec4 const Mat44::GetZDimension4D() const
{
	return Vec4(m_value[Iz], m_value[Jz], m_value[Kz], m_value[Tz]);
}

Vec4 const Mat44::GetWDimension4D() const
{
	return Vec4(m_value[Iw], m_value[Jw], m_value[Kw], m_value[Tw]);
}

void Mat44::SetTranslation2D(Vec2 const& translationXY)
{
	m_value[Tx] = translationXY.x;
	m_value[Ty] = translationXY.y;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;
}

void Mat44::SetTranslation3D(Vec3 const& translationXYZ)
{
	m_value[Tx] = translationXYZ.x;
	m_value[Ty] = translationXYZ.y;
	m_value[Tz] = translationXYZ.z;
	m_value[Tw] = 1.f;
}

void Mat44::SetIJ2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D)
{
	m_value[Ix] = iBasis2D.x;
	m_value[Iy] = iBasis2D.y;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

 	m_value[Jx] = jBasis2D.x;
	m_value[Jy] = jBasis2D.y;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;
}

void Mat44::SetIJT2D(Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY)
{
	m_value[Ix] = iBasis2D.x;
	m_value[Iy] = iBasis2D.y;
	m_value[Iz] = 0.f;
	m_value[Iw] = 0.f;

	m_value[Jx] = jBasis2D.x;
	m_value[Jy] = jBasis2D.y;
	m_value[Jz] = 0.f;
	m_value[Jw] = 0.f;

	m_value[Tx] = translationXY.x;
	m_value[Ty] = translationXY.y;
	m_value[Tz] = 0.f;
	m_value[Tw] = 1.f;
}

void Mat44::SetIJK3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D)
{
	m_value[Ix] = iBasis3D.x;
	m_value[Iy] = iBasis3D.y;
	m_value[Iz] = iBasis3D.z;
	m_value[Iw] = 0.f;

	m_value[Jx] = jBasis3D.x;
	m_value[Jy] = jBasis3D.y;
	m_value[Jz] = jBasis3D.z;
	m_value[Jw] = 0.f;

	m_value[Kx] = kBasis3D.x;
	m_value[Ky] = kBasis3D.y;
	m_value[Kz] = kBasis3D.z;
	m_value[Kw] = 0.f;
}

void Mat44::SetIJKT3D(Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ)
{
	m_value[Ix] = iBasis3D.x;
	m_value[Iy] = iBasis3D.y;
	m_value[Iz] = iBasis3D.z;
	m_value[Iw] = 0.f;

	m_value[Jx] = jBasis3D.x;
	m_value[Jy] = jBasis3D.y;
	m_value[Jz] = jBasis3D.z;
	m_value[Jw] = 0.f;

	m_value[Kx] = kBasis3D.x;
	m_value[Ky] = kBasis3D.y;
	m_value[Kz] = kBasis3D.z;
	m_value[Kw] = 0.f;

	m_value[Tx] = translationXYZ.x;
	m_value[Ty] = translationXYZ.y;
	m_value[Tz] = translationXYZ.z;
	m_value[Tw] = 1.f;
}

void Mat44::SetIJKT4D(Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D)
{
	m_value[Ix] = iBasis4D.x;
	m_value[Iy] = iBasis4D.y;
	m_value[Iz] = iBasis4D.z;
	m_value[Iw] = iBasis4D.w;
						
	m_value[Jx] = jBasis4D.x;
	m_value[Jy] = jBasis4D.y;
	m_value[Jz] = jBasis4D.z;
	m_value[Jw] = jBasis4D.w;
						
	m_value[Kx] = kBasis4D.x;
	m_value[Ky] = kBasis4D.y;
	m_value[Kz] = kBasis4D.z;
	m_value[Kw] = kBasis4D.w;

	m_value[Tx] = translation4D.x;
	m_value[Ty] = translation4D.y;
	m_value[Tz] = translation4D.z;
	m_value[Tw] = translation4D.w;
}

void Mat44::Transpose()
{
	SwapFloatValue(m_value[Jx], m_value[Iy]);
	SwapFloatValue(m_value[Kx], m_value[Iz]);
	SwapFloatValue(m_value[Tx], m_value[Iw]);
	SwapFloatValue(m_value[Jz], m_value[Ky]);
	SwapFloatValue(m_value[Jw], m_value[Ty]);
	SwapFloatValue(m_value[Kw], m_value[Tz]);
}

void Mat44::Orthonormalize_XFwd_YLeft_ZUp()
{
	Vec3 i = Vec3(m_value[Ix], m_value[Iy], m_value[Iz]);
	Vec3 j = Vec3(m_value[Jx], m_value[Jy], m_value[Jz]);
	Vec3 k = Vec3(m_value[Kx], m_value[Ky], m_value[Kz]);
	i.Normalize();
	Vec3 ki = DotProduct3D(i, k) * i;
	k = k - ki;
	k.Normalize();
	Vec3 ji = DotProduct3D(i, j) * i;
	j = j - ji;
	Vec3 jk = DotProduct3D(k, j) * k;
	j = j - jk;
	j.Normalize();
	SetIJK3D(i, j, k);
}

void Mat44::Append(Mat44 const& appendThis)
{
	Mat44 copy(&m_value[0]);
	m_value[Ix] = DotProduct4D(copy.GetXDimension4D(), appendThis.GetIBasis4D());
	m_value[Iy] = DotProduct4D(copy.GetYDimension4D(), appendThis.GetIBasis4D());
	m_value[Iz] = DotProduct4D(copy.GetZDimension4D(), appendThis.GetIBasis4D());
	m_value[Iw] = DotProduct4D(copy.GetWDimension4D(), appendThis.GetIBasis4D());

	m_value[Jx] = DotProduct4D(copy.GetXDimension4D(), appendThis.GetJBasis4D());
	m_value[Jy] = DotProduct4D(copy.GetYDimension4D(), appendThis.GetJBasis4D());
	m_value[Jz] = DotProduct4D(copy.GetZDimension4D(), appendThis.GetJBasis4D());
	m_value[Jw] = DotProduct4D(copy.GetWDimension4D(), appendThis.GetJBasis4D());

	m_value[Kx] = DotProduct4D(copy.GetXDimension4D(), appendThis.GetKBasis4D());
	m_value[Ky] = DotProduct4D(copy.GetYDimension4D(), appendThis.GetKBasis4D());
	m_value[Kz] = DotProduct4D(copy.GetZDimension4D(), appendThis.GetKBasis4D());
	m_value[Kw] = DotProduct4D(copy.GetWDimension4D(), appendThis.GetKBasis4D());

	m_value[Tx] = DotProduct4D(copy.GetXDimension4D(), appendThis.GetTranslation4D());
	m_value[Ty] = DotProduct4D(copy.GetYDimension4D(), appendThis.GetTranslation4D());
	m_value[Tz] = DotProduct4D(copy.GetZDimension4D(), appendThis.GetTranslation4D());
	m_value[Tw] = DotProduct4D(copy.GetWDimension4D(), appendThis.GetTranslation4D());
}

void Mat44::AppendZRotation(float degreesRotationAboutZ)
{
	Append(CreateZRotationDegrees(degreesRotationAboutZ));
}

void Mat44::AppendYRotation(float degreesRotationAboutY)
{
	Append(CreateYRotationDegrees(degreesRotationAboutY));
}

void Mat44::AppendXRotation(float degreesRotationAboutX)
{
	Append(CreateXRotationDegrees(degreesRotationAboutX));
}

void Mat44::AppendTranslation2D(Vec2 const& translationXY)
{
	Mat44 mat;
	mat.SetTranslation2D(translationXY);
	Append(mat);
}

void Mat44::AppendTranslation3D(Vec3 const& translationXYZ)
{
	Mat44 mat;
	mat.SetTranslation3D(translationXYZ);
	Append(mat);
}

void Mat44::AppendScaleUniform2D(float uniformScaleXY)
{
	Append(CreateUniformScale2D(uniformScaleXY));
}

void Mat44::AppendScaleUniform3D(float uniformScaleXYZ)
{
	Append(CreateUniformScale3D(uniformScaleXYZ));
}

void Mat44::AppendScaleNonUniform2D(Vec2 const& nonUniformScaleXY)
{
	Append(CreateNonUniformScale2D(nonUniformScaleXY));
}

void Mat44::AppendScaleNonUniform3D(Vec3 const& nonUniformScaleXYZ)
{
	Append(CreateNonUniformScale3D(nonUniformScaleXYZ));
}

float Mat44::Determinant3x3(float a, float b, float c, float d, float e, float f, float g, float h, float i) const
{
	return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
}

float Mat44::Determinant() const
{
	return m_value[0] * Determinant3x3(m_value[5], m_value[6], m_value[7], m_value[9], m_value[10], m_value[11], m_value[13], m_value[14], m_value[15]) -
		m_value[1] * Determinant3x3(m_value[4], m_value[6], m_value[7], m_value[8], m_value[10], m_value[11], m_value[12], m_value[14], m_value[15]) +
		m_value[2] * Determinant3x3(m_value[4], m_value[5], m_value[7], m_value[8], m_value[9], m_value[11], m_value[12], m_value[13], m_value[15]) -
		m_value[3] * Determinant3x3(m_value[4], m_value[5], m_value[6], m_value[8], m_value[9], m_value[10], m_value[12], m_value[13], m_value[14]);
}

Mat44 const Mat44::GetInversedMatrix() const
{
	Mat44 result;
	float det = this->Determinant();
	if (Absf(det) < 1e-6f) {
		// Matrix is singular, cannot compute the inverse
		// You might want to handle this case differently based on your application's requirements
		// For example, returning an identity matrix or throwing an exception
		// For now, just return the original matrix
		return *this;
	}
	float invDet = 1.0f / det;
	result.m_value[0] = Determinant3x3(		m_value[5], m_value[6], m_value[7], m_value[9], m_value[10],	m_value[11],	m_value[13],	m_value[14], m_value[15]) * invDet;
	result.m_value[1] = -Determinant3x3(	m_value[1], m_value[2], m_value[3], m_value[9], m_value[10],	m_value[11],	m_value[13],	m_value[14], m_value[15]) * invDet;
	result.m_value[2] = Determinant3x3(		m_value[1], m_value[2], m_value[3], m_value[5], m_value[6],		m_value[7],		m_value[13],	m_value[14], m_value[15]) * invDet;
	result.m_value[3] = -Determinant3x3(	m_value[1], m_value[2], m_value[3], m_value[5], m_value[6],		m_value[7],		m_value[9],		m_value[10], m_value[11]) * invDet;
	result.m_value[4] = -Determinant3x3(	m_value[4], m_value[6], m_value[7], m_value[8], m_value[10],	m_value[11],	m_value[12],	m_value[14], m_value[15]) * invDet;
	result.m_value[5] = Determinant3x3(		m_value[0], m_value[2], m_value[3], m_value[8], m_value[10],	m_value[11],	m_value[12],	m_value[14], m_value[15]) * invDet;
	result.m_value[6] = -Determinant3x3(	m_value[0], m_value[2], m_value[3], m_value[4], m_value[6],		m_value[7],		m_value[12],	m_value[14], m_value[15]) * invDet;
	result.m_value[7] = Determinant3x3(		m_value[0], m_value[2], m_value[3], m_value[4], m_value[6],		m_value[7],		m_value[8],		m_value[10], m_value[11]) * invDet;
	result.m_value[8] = Determinant3x3(		m_value[4], m_value[5], m_value[7], m_value[8], m_value[9],		m_value[11],	m_value[12],	m_value[13], m_value[15]) * invDet;
	result.m_value[9] = -Determinant3x3(	m_value[0], m_value[1], m_value[3], m_value[8], m_value[9],		m_value[11],	m_value[12],	m_value[13], m_value[15]) * invDet;
	result.m_value[10] = Determinant3x3(	m_value[0], m_value[1], m_value[3], m_value[4], m_value[5],		m_value[7],		m_value[12],	m_value[13], m_value[15]) * invDet;
	result.m_value[11] = -Determinant3x3(	m_value[0], m_value[1], m_value[3], m_value[4], m_value[5],		m_value[7],		m_value[8],		m_value[9],  m_value[11]) * invDet;
	result.m_value[12] = -Determinant3x3(	m_value[4], m_value[5], m_value[6], m_value[8], m_value[9],		m_value[10],	m_value[12],	m_value[13], m_value[14]) * invDet;
	result.m_value[13] = Determinant3x3(	m_value[0], m_value[1], m_value[2], m_value[8], m_value[9],		m_value[10],	m_value[12],	m_value[13], m_value[14]) * invDet;
	result.m_value[14] = -Determinant3x3(	m_value[0], m_value[1], m_value[2], m_value[4], m_value[5],		m_value[6],		m_value[12],	m_value[13], m_value[14]) * invDet;
	result.m_value[15] = Determinant3x3(	m_value[0], m_value[1], m_value[2], m_value[4], m_value[5],		m_value[6],		m_value[8],		m_value[9],  m_value[10]) * invDet;
	return result;
}