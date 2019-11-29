#include "camera.h"

#include <iostream>

#include "external-files/glew/glew.h"
#include "external-files/glm/gtc/matrix_transform.hpp"

static const float TERMINAL = 100.0, ACC = 50.0, DAMPEN = 3.0;
static const float ANGLE_X = 0.2, ANGLE_Y = 0.1;

using namespace std;

Camera::Camera(int winx, int winy)
	: _fov(45.0 * M_PI / 180.0),
	  _near(0.1),
	  _far(200.0),
	  _ratio(1.0 * winx / winy),

	  _pos(glm::vec3(0.0, 0.0, 0.0)),
	  _vel(glm::vec3(0.0, 0.0, 0.0)),
	  _front(glm::vec3(0.0, 0.0, -1.0)),
	  _axis(glm::vec3(0.0, 1.0, 0.0)),

	  _rot(1.0, 0.0, 0.0, 0.0),

	  _mouseX(0.0),
	  _mouseY(0.0),

	  forward(false),
	  back(false),
	  left(false),
	  right(false),
	  up(false),
	  down(false),
	  slow(false)
{
	// Load in initial viewing maticies
	loadView();
	loadPerp();
}

Camera::~Camera() {}

void Camera::update(float delta)
{
	glm::vec3 ortho = glm::normalize(glm::cross(_front, _axis));

	// Update position
	_pos += _vel * delta;

	// For finer movements, reduce camera acceleration
	float precision = 1.0;
	if (slow)
		precision = 0.1;

	updatePosition(forward, back, delta, precision, _front);
	updatePosition(left, right, delta, precision, -ortho);
	updatePosition(down, up, delta, precision, -_axis);

	// Update view rotations
	// Note: This has obvious issues, camera tends to roll when it shouldn't
	if (_mouseX || _mouseY) {
		glm::f32quat tmp(1.0, 0.0, 0.0, 0.0);
		if (_mouseX)
			tmp = glm::angleAxis(-_mouseX/500, _axis) * tmp;
		if (_mouseY)
			tmp = glm::angleAxis(-_mouseY/500, ortho) * tmp;

		_rot = glm::normalize(tmp * _rot);
		//_front = glm::normalize(_rot * glm::vec3(0.0, 0.0, -1.0));
	}

	// Reset mouse pos
	_mouseX = 0;
	_mouseY = 0;

	loadView();
}

void Camera::updatePosition(bool a, bool b, float t, float p, glm::vec3 axis)
{
	// Check if only in one direction
	// Else slow down in said axis
	if (a ^ b) {
		// If speed is less than terminal velocity
		if (glm::length(_vel * axis) < TERMINAL * p) {
			if (a)
				_vel += ACC * axis * t * p;
			else
				_vel -= ACC * axis * t * p;
		}
	} else {
		_vel += (_vel * axis) * DAMPEN * t;
	}
}

void Camera::look(int x, int y)
{
	_mouseX -= x * ANGLE_X;
	_mouseY -= y * ANGLE_Y;
}

void Camera::reshape(int x, int y)
{
	_ratio = 1.0 * x / y;
	loadPerp();
}

void Camera::setPerp(float fov, float near, float far)
{
	_fov = fov * M_PI / 180.0;
	_near = near;
	_far = far;
	loadPerp();
}

void Camera::setPos(glm::vec3 p)
{
	_pos = p;
	loadView();
}

void Camera::setPos(float x, float y, float z)
{
	Camera::setPos(glm::vec3(x, y, z));
}

std::vector<glm::vec3> Camera::getFrustumNormals()
{
	vector<glm::vec3> normals;
	glm::vec3 ortho = glm::normalize(glm::cross(_front, _axis));

	float nh = _near * glm::tan(0.5 * _fov);
	float nw = nh * _ratio;

	// Calculate frustum normals
	glm::vec3 nc = _pos + _front * _near;

	normals.push_back(glm::normalize((nc + _axis * nh) - _pos) * ortho); // Top
	normals.push_back(ortho * glm::normalize((nc - _axis * nh) - _pos)); // Bottom

	normals.push_back(glm::normalize((nc - ortho * nw) - _pos) * _axis); // Left
	normals.push_back(_axis * glm::normalize((nc + ortho * nw) - _pos)); // Right

	return normals;
}


void Camera::loadView()
{
	// Reload the view matrix
	_view = glm::translate(glm::mat4_cast(_rot), -_pos);
}

void Camera::loadPerp()
{
	// Reload perspective matrix
	_perp = glm::perspective(_fov, _ratio, _near, _far);
}

glm::mat4 Camera::getView() const { return _view; }
glm::mat4 Camera::getPerp() const { return _perp; }
glm::vec3 Camera::getPos() const { return _pos; }
glm::vec3 Camera::getViewDir() const { return _front; }