
#include "pch.h"
#include "ArcballCamera.hpp"

namespace cst {

	//
	// Private API
	//

	// update position, orientation and view matrices when camera rotation and radius is modified
	void ArcballCamera::calculateDerivedValues() {

		const float theta_ = glm::radians<float>(theta);
		const float phi_ = glm::radians<float>(phi);

		// calculate position vector C
		C = glm::vec4(sinf(phi_) * cosf(-theta_) * r, sinf(-theta_) * r, cosf(phi_) * cosf(-theta_) * r, 1.0f);

		// calculate orientation basis R
		R = glm::eulerAngleY(phi_) * glm::eulerAngleX(theta_);
		//R = GUMatrix4::rotationMatrix(0.0f, phi_, 0.0f) * GUMatrix4::rotationMatrix(theta_, 0.0f, 0.0f);

		// calculate view transformation matrix V
		V = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -r)) * glm::eulerAngleX(-theta_) * glm::eulerAngleY(-phi_);
		//V = GUMatrix4::translationMatrix(0.0f, 0.0f, -r) * GUMatrix4::rotationMatrix(-theta_, 0.0f, 0.0f) * GUMatrix4::rotationMatrix(0.0f, -phi_, 0.0f);

		// calculate inverse view transform Vinv
		Vinv = glm::eulerAngleY(phi_) * glm::eulerAngleX(theta_) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, r));
		//Vinv = GUMatrix4::rotationMatrix(0.0f, phi_, 0.0f) * GUMatrix4::rotationMatrix(theta_, 0.0f, 0.0f) * GUMatrix4::translationMatrix(0.0f, 0.0f, r);
	}

	//
	// Public method implementation
	//

	// ArcballCamera constructors

	// initialise camera parameters so it is placed at the origin looking down the -z axis (for a right-handed camera) or +z axis (for a left-handed camera)
	ArcballCamera::ArcballCamera() {

		theta = 0.0f;
		phi = 0.0f;
		r = 15.0f;

		F = ViewFrustum(55.0f, 1.0f, 0.1f, 500.0f);

		// calculate derived values
		calculateDerivedValues();
		F.calculateWorldCoordPlanes(C, R);
	}


	// create a camera with orientation <theta, phi> representing Euler angles specified in degrees and Euclidean distance 'init_radius' from the origin.  The frustum / viewplane projection coefficients are defined in init_fovy, specified in degrees spanning the entire vertical field of view angle, init_aspect (w/h ratio), init_nearPlane and init_farPlane.  If init_farPlane = 0.0 (as determined by equalf) then the resulting frustum represents an infinite perspective projection.  This is the default
	ArcballCamera::ArcballCamera(const float init_theta, const float init_phi, const float init_radius, const float init_fovy, const float init_aspect, const float init_nearPlane, const float init_farPlane) {

		theta = init_theta;
		phi = init_phi;

		r = init_radius;

		if (r < 0.0f)
			r = 0.0f;

		F = ViewFrustum(init_fovy, init_aspect, init_nearPlane, init_farPlane);

		// calculate derived values
		calculateDerivedValues();
		F.calculateWorldCoordPlanes(C, R);
	}


	float ArcballCamera::getRadius() {

		return r;
	}

	void ArcballCamera::scaleRadius(float s) {

		r *= s;

		// calculate derived values
		calculateDerivedValues();
		F.calculateWorldCoordPlanes(C, R);
	}

	void ArcballCamera::incrementRadius(float i) {

		r += std::max<float>(r + i, 0.0f);
		
		// calculate derived values
		calculateDerivedValues();
		F.calculateWorldCoordPlanes(C, R);
	}

	void ArcballCamera::rotateCamera(float dTheta, float dPhi) {
		theta += dTheta;
		phi += dPhi;

		// calculate derived values
		calculateDerivedValues();
		F.calculateWorldCoordPlanes(C, R);
	}


	// Accessor methods

	// return the pivot rotation around the x axis (theta) in degrees
	float ArcballCamera::getTheta() const {

		return theta;
	}

	// return the pivot rotation around the y axis (phi) in degrees
	float ArcballCamera::getPhi() const {

		return phi;
	}

	// return a const reference to the camera's view frustum
	const ViewFrustum& ArcballCamera::getViewFrustum() const {

		return F;
	}

	// return the camera location in world coordinate space
	glm::vec4 ArcballCamera::getPosition() const {

		return C;
	}

	// return a const reference to the camera's orientation matrix in world coordinate space
	const glm::mat4& ArcballCamera::getOrientation() const {

		return R;
	}

	// return a const reference to the view transform matrix for the camera
	const glm::mat4& ArcballCamera::viewTransform() const {

		return V;
	}

	// return a const reference to the matrix that maps the camera from eye coordinate space to world coordinate space
	const glm::mat4& ArcballCamera::inverseViewTransform() const {

		return Vinv;
	}


	// return a const reference the projection transform for the camera
	const glm::mat4& ArcballCamera::projectionTransform() const {

		return F.projectionMatrix();
	}


	// camera projection setter methods - these act as pass-through methods to the encapsulated GUViewFrustum.  Corresponding reader methods are not specified as these can be accessed via the const interface provided through the GUViewFrustum accessor specified above.  These methods also recalculate the world coordinate planes of the encapsulated frustum

	// set fovy to newFovy specified in degrees and recalculate derived frustum values.  newFovy represents the entire vertical field of view
	void ArcballCamera::setFieldOfView(const float newFovy) {

		F.setFieldOfView(newFovy);
		F.calculateWorldCoordPlanes(C, R);
	}


	// set aspect (w/h) ratio to newAspect and recalculate derived frustum values
	void ArcballCamera::setAspectRatio(const float newAspect) {

		F.setAspectRatio(newAspect);
		F.calculateWorldCoordPlanes(C, R);
	}


	// set the near plane distance to newDistance and recalculate derived frustum values.  newDistance > 0 is assumed
	void ArcballCamera::setNearPlaneDistance(const float newDistance) {

		F.setNearPlaneDistance(newDistance);
		F.calculateWorldCoordPlanes(C, R);
	}


	// set the far plane distance to newDistance and recalculate derived frustum values.  If newDistance = 0.0 (as determined by equalf) the encapsulated frustum represents an infinite perspective projection.  newDistance >= 0 is assumed
	void ArcballCamera::setFarPlaneDistance(const float newDistance) {

		F.setFarPlaneDistance(newDistance);
		F.calculateWorldCoordPlanes(C, R);
	}
}