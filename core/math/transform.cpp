/*************************************************************************/
/*  transform.cpp                                                        */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                 */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "transform.h"
#include "math_funcs.h"
#include "os/copymem.h"
#include "print_string.h"
	

void Transform::affine_invert() {

	basis.invert();
	origin = basis.xform(-origin);
}

Transform Transform::affine_inverse() const {

	Transform ret=*this;
	ret.affine_invert();
	return ret;

}


void Transform::invert() {

	basis.transpose();
	origin = basis.xform(-origin);	
}

Transform Transform::inverse() const {

	Transform ret=*this;
	ret.invert();
	return ret;
}


void Transform::rotate(const Vector3& p_axis,real_t p_phi) {

	*this = *this * Transform( Matrix3( p_axis, p_phi ), Vector3()  );
}

Transform Transform::rotated(const Vector3& p_axis,real_t p_phi) const{

	return *this * Transform( Matrix3( p_axis, p_phi ), Vector3()  );
}

void Transform::rotate_basis(const Vector3& p_axis,real_t p_phi) {

	basis.rotate(p_axis,p_phi);
}

Transform Transform::looking_at( const Vector3& p_target, const Vector3& p_up ) const {

	Transform t = *this;
	t.set_look_at(origin,p_target,p_up);
	return t;
}

void Transform::set_look_at( const Vector3& p_eye, const Vector3& p_target, const Vector3& p_up ) {

	// Reference: MESA source code
	Vector3 v_x, v_y, v_z;
	
	/* Make rotation matrix */
	
	/* Z vector */
	v_z = p_eye - p_target;
	
	v_z.normalize();
	
	v_y = p_up;
	

	v_x=v_y.cross(v_z);
	
	/* Recompute Y = Z cross X */
	v_y=v_z.cross(v_x);
	
	v_x.normalize();
	v_y.normalize();
	
	basis.set_axis(0,v_x);
	basis.set_axis(1,v_y);
	basis.set_axis(2,v_z);
	origin=p_eye;
	
}

Transform Transform::interpolate_with(const Transform& p_transform, float p_c) const {

	/* not sure if very "efficient" but good enough? */

	Vector3 src_scale = basis.get_scale();
	Quat src_rot = basis;
	Vector3 src_loc = origin;

	Vector3 dst_scale = p_transform.basis.get_scale();
	Quat dst_rot = p_transform.basis;
	Vector3 dst_loc = p_transform.origin;

	Transform dst;
	dst.basis=src_rot.slerp(dst_rot,p_c);
	dst.basis.scale(src_scale.linear_interpolate(dst_scale,p_c));
	dst.origin=src_loc.linear_interpolate(dst_loc,p_c);

	return dst;
}

void Transform::scale(const Vector3& p_scale) {

	basis.scale(p_scale);
	origin*=p_scale;
}

Transform Transform::scaled(const Vector3& p_scale) const {

	Transform t = *this;
	t.scale(p_scale);
	return t;
}

void Transform::scale_basis(const Vector3& p_scale) {

	basis.scale(p_scale);
}

void Transform::translate( real_t p_tx, real_t p_ty, real_t p_tz)  {
	translate( Vector3(p_tx,p_ty,p_tz) );

}
void Transform::translate( const Vector3& p_translation ) {

	for( int i = 0; i < 3; i++ ) {
		origin[i] += basis[i].dot(p_translation);
	}
}

Transform Transform::translated( const Vector3& p_translation ) const {

	Transform t=*this;
	t.translate(p_translation);
	return t;
}

void Transform::orthonormalize() {

	basis.orthonormalize();
}

Transform Transform::orthonormalized() const {

	Transform _copy = *this;
	_copy.orthonormalize();
	return _copy;
}

bool Transform::operator==(const Transform& p_transform) const {

	return (basis==p_transform.basis && origin==p_transform.origin);
}
bool Transform::operator!=(const Transform& p_transform) const {

	return (basis!=p_transform.basis || origin!=p_transform.origin);
}

void Transform::operator*=(const Transform& p_transform) {

	origin=xform(p_transform.origin);
	basis*=p_transform.basis;	
}

Transform Transform::operator*(const Transform& p_transform) const {

	Transform t=*this;
	t*=p_transform;
	return t;
}

Transform::operator String() const {

	return basis.operator String() + " - " + origin.operator String();
}


Transform::Transform(const Matrix3& p_basis, const Vector3& p_origin) {

	basis=p_basis;
	origin=p_origin;
}


