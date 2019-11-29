#ifndef CAMERA_H
#define CAMERA_H

#include <vector>

#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include "external-files/glm/glm.hpp"
#include "external-files/glm/gtc/quaternion.hpp"

class Camera
{
	public:
		Camera(int winx, int winy);
		~Camera();

		// Update camera position and orientation after delta time
		void update(float delta);

		// Set the amount to yaw/pitch camera next update
		void look(int x, int y);

		// Set new screen dimentions
		void reshape(int x, int y);
		void setPerp(float fov, float near, float far);

		void setPos(glm::vec3 p);
		void setPos(float x, float y, float z);

		std::vector<glm::vec3> getFrustumNormals();

		glm::mat4 getView() const;
		glm::mat4 getPerp() const;
		glm::vec3 getPos() const;
		glm::vec3 getViewDir() const;

		// Movement flags
		bool forward, back;
		bool left, right;
		bool up, down;
		bool slow;
		
	private:
		// Store current viewing maticies
		glm::mat4 _view, _perp;

		// Persepctive data
		float _fov, _ratio, _near, _far;
		
		// Position/orientation data
		glm::vec3 _pos, _vel, _front, _axis;
		glm::f32quat _rot;

		// Amount to rotate camera next update
		float _mouseX, _mouseY;

		// Reload camera maticies
		void loadView();
		void loadPerp();

		void updatePosition(bool a, bool b, float t, float p, glm::vec3 axis);
};

#endif