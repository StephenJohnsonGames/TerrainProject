
#include "pch.h"
#include "ViewFrustum.hpp"


using namespace std;


namespace cst {


	//
	// Private API implementation
	//

	// given the current projection coefficients, calculate the derived quantity flength (focal length)
	void ViewFrustum::calculateFocalLength() {
		
		flength = 1.0f / tanf(glm::radians<float>(fovy) / 2.0f);
	}


	// calculate the projection matrix for the current projection coefficients.  The matrix is stored in column major format.  An infinite perspective projection matrix is setup if d_far = 0.0 as determined by equalf
	void ViewFrustum::calculateProjectionMatrix() {
		
		//if (isInfinite())
		//	P = glm::infinitePerspective(fovy, aspect, d_near);
		//P = GUMatrix4::infinitePerspectiveProjection(fovy, aspect, d_near);
		//else
		P = glm::perspective(glm::radians<float>(fovy), aspect, d_near, d_far);
		//P = GUMatrix4::perspectiveProjection(fovy, aspect, d_near, d_far);
	}


	// calculate frustum planes in eye coordinate space based on the current projection coefficients.  The resulting plane normal vectors are normalised to unit length.  For OpenGL, by default the frustum and therefore the planes are oriented along the negative z axis.   Note:  The plane equations below are based on Lengyel's model on p. 116 (2nd ed.)  However Lengyel uses h/w aspect and fovx rather than w/h aspect and fovy as used here.  Therefore the bottom and top become the left and right vectors and visa versa.  The x and y components are also swapped (compare below with table 4.1 in the above reference).  For a frustum representing an infinite perspective projection the far plane coefficients represent a 'placeholder' plane offset 1.0 from the near plane.  This should not be used for view frustum intersection testing and is only setup for testing / frustum visualisation purposes
	void ViewFrustum::calculateEyeCoordPlanes() {

		float e = flength;
		float a = aspect;

		float dl = sqrtf(e * e + a * a);
		float db = sqrtf(e * e + 1);

		// right-handed orientation (OpenGL) places the frustum along the -z axis in eye coordinates
		eLeft = glm::vec4(e / dl, 0.0f, -a / dl, 0.0f);
		eRight = glm::vec4(-eLeft.x, 0.0f, eLeft.z, 0.0f);
		eBottom = glm::vec4(0.0f, e / db, -1.0f / db, 0.0f);
		eTop = glm::vec4(0.0f, -eBottom.y, eBottom.z, 0.0f);
		eNear = glm::vec4(0.0f, 0.0f, -1.0f, -d_near);
		eFar = glm::vec4(0.0f, 0.0f, 1.0f, (isInfinite() ? d_near + 1.0f : d_far));
	}


	//
	// CGViewFrustum public method implementation
	//


	// default constructor - initialise projection coefficients to a basic camera model where fovy=55.0f, aspect = 1.0f, d_near = 0.1f and d_far = 500.0f
	ViewFrustum::ViewFrustum() {

		fovy = 55.0f;
		aspect = 1.0f;
		d_near = 0.1f;
		d_far = 500.0f;

		// calculate derived values
		calculateFocalLength();
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	// initialise the view frustum and set the projection matrix to the infinite projection matrix if farInit = 0.0 as determined by equalf (this is the default)
	ViewFrustum::ViewFrustum(const float fovyInit, const float aspectInit, const float nearInit, const float farInit) {

		fovy = fovyInit;
		aspect = aspectInit;
		d_near = nearInit;
		d_far = farInit;

		// calculate derived values
		calculateFocalLength();
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	// Accessor methods

	const glm::mat4& ViewFrustum::projectionMatrix() const {

		return P;
	}


	float ViewFrustum::verticalFieldOfView() const {

		return fovy;
	}


	// set fovy to newFovy specified in degrees and recalculate derived frustum values.  newFovy represents the entire vertical field of view
	void ViewFrustum::setFieldOfView(const float newFovy) {

		fovy = newFovy;

		// calculate derived values
		calculateFocalLength();
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	float ViewFrustum::aspectRatio() const {

		return aspect;
	}


	// set aspect (w/h) ratio to newAspect and recalculate derived frustum values
	void ViewFrustum::setAspectRatio(const float newAspect) {

		aspect = newAspect;

		// calculate derived values (focal length flength does not need recalculating here)
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	float ViewFrustum::focalLength() const {

		return flength;
	}


	float ViewFrustum::nearPlaneDistance() const {

		return d_near;
	}


	// set the near plane distance to newDistance and recalculate derived frustum values.  newDistance > 0 is assumed
	void ViewFrustum::setNearPlaneDistance(const float newDistance) {

		d_near = newDistance;

		// calculate derived values (focal length flength does not need recalculating here)
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	float ViewFrustum::farPlaneDistance() const {

		return d_far;
	}


	// set the far plane distance to newDistance and recalculate derived frustum values.  If newDistance = 0.0 (as determined by equalf) the frustum represents an infinite perspective projection.  newDistance >= 0 is assumed
	void ViewFrustum::setFarPlaneDistance(const float newDistance) {

		d_far = newDistance;

		// calculate derived values (focal length flength does not need recalculating here)
		calculateProjectionMatrix();
		calculateEyeCoordPlanes();
	}


	// return true if d_far = 0.0 (as determined by fequal) and the frustum represents an infinite perspective projection, false otherwise
	bool ViewFrustum::isInfinite() const {

		return cst::fequal(d_far, 0.0f);
	}


	// return the frustum planes in eye coordinates via the specified pointers.  All provided pointers are assumed to be valid
	void ViewFrustum::getEyeCoordPlanes(glm::vec4* eLeft_, glm::vec4* eRight_, glm::vec4* eTop_, glm::vec4* eBottom_, glm::vec4* eNear_, glm::vec4* eFar_) const {

		*eLeft_ = eLeft;
		*eRight_ = eRight;
		*eTop_ = eTop;
		*eBottom_ = eBottom;
		*eNear_ = eNear;
		*eFar_ = eFar;
	}

	// return the frustum planes in world coordinates via the specified pointers.  All provided pointers are assumed to be valid
	void ViewFrustum::getWorldCoordPlanes(glm::vec4* wLeft_, glm::vec4* wRight_, glm::vec4* wTop_, glm::vec4* wBottom_, glm::vec4* wNear_, glm::vec4* wFar_) const {

		*wLeft_ = wLeft;
		*wRight_ = wRight;
		*wTop_ = wTop;
		*wBottom_ = wBottom;
		*wNear_ = wNear;
		*wFar_ = wFar;
	}


	// Frustum plane methods

	// Given camera position C and orientation R in R3, calculate the view frustum planes in world coordinate space.  Given R is the orientation of the camera, R^-1 = R^T is used to rotate this to eye coordinate space.  Let T^-1 be translate(-C.x, -C.y, -C.z) be the translation from the camera world coordinate to eye coordinate space.  The view transform is therefore (R^T * T^-1).  The inverse of this is therefore T * R which maps from eye coordinate space to world coordinate space.  The inverse transpose of T * R is actually W = (R^-1 * T^-1)^T.  This inverse transpose is used to map the frustum plane normals from eye coordinate space to world coordinate space.  The frustum planes are normalised so |abc| = 1.0 for the plane coefficients <a, b, c, d>
	void ViewFrustum::calculateWorldCoordPlanes(const glm::vec4& C, const glm::mat4& R) {
		glm::mat4 W = glm::transpose(glm::transpose(R) *
		glm::translate(glm::mat4(1.0f), glm::vec3(-C.x, -C.y, -C.z)));
		//GUMatrix4 W = (R.transpose() * GUMatrix4::translationMatrix(-C.x, -C.y, -C.z)).transpose();

		wLeft = W * eLeft;
		wRight = W * eRight;
		wBottom = W * eBottom;
		wTop = W * eTop;
		wNear = W * eNear;
		wFar = W * eFar;
	}


	// return true if the given point P lies inside the frustum, false otherwise.  P is assumed to be in world coordinate space and it is assumed the world coordinate planes of the frustum have already been calculated with a previous call to CGViewFrustum::calculateWorldCoordPlanes.  If d_far = 0.0 (an infinite perspective projection is used) then the far plane is ignored in determining whether the point lies within the frustum
	bool ViewFrustum::pointInFrustum(const glm::vec4& P) const {

		// test P against each plane in turn
		
		if (glm::dot(wLeft, P) + wLeft.w < 0.0f)
			return false;

		if (glm::dot(wRight, P) + wRight.w < 0.0f)
			return false;

		if (glm::dot(wBottom, P) + wBottom.w < 0.0f)
			return false;

		if (glm::dot(wTop, P) + wTop.w < 0.0f)
			return false;

		if (glm::dot(wNear, P) + wNear.w < 0.0f)
			return false;

		if (!isInfinite()) {

			if (glm::dot(wFar, P) + wFar.w < 0.0f)
				return false;
		}

		// if all planes pass then P lies inside the frustum
		return true;
	}

}
