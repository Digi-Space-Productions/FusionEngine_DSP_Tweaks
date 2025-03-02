/*************************************************************************/
/*  joints_2d.cpp                                                        */
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
#include "joints_2d.h"
#include "servers/physics_2d_server.h"
#include "physics_body_2d.h"

void Joint2D::_update_joint() {

	if (!is_inside_tree())
		return;

	if (joint.is_valid()) {
		Physics2DServer::get_singleton()->free(joint);
	}

	joint=RID();


	joint = _configure_joint();
	Physics2DServer::get_singleton()->get_singleton()->joint_set_param(joint,Physics2DServer::JOINT_PARAM_BIAS,bias);


}


void Joint2D::set_node_a(const NodePath& p_node_a) {


	if (a==p_node_a)
		return;

	a=p_node_a;
	_update_joint();
}

NodePath Joint2D::get_node_a() const{

	return a;
}

void Joint2D::set_node_b(const NodePath& p_node_b){

	if (b==p_node_b)
		return;
	b=p_node_b;
	_update_joint();

}
NodePath Joint2D::get_node_b() const{


	return b;
}


void Joint2D::_notification(int p_what) {

	switch(p_what) {

		case NOTIFICATION_READY: {
			_update_joint();
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (joint.is_valid()) {

				Physics2DServer::get_singleton()->free(joint);
				joint=RID();
			}
		} break;

	}

}

void Joint2D::set_bias(real_t p_bias) {

	bias=p_bias;
	if (joint.is_valid())
		Physics2DServer::get_singleton()->get_singleton()->joint_set_param(joint,Physics2DServer::JOINT_PARAM_BIAS,bias);
}

real_t Joint2D::get_bias() const{


	return bias;
}


void Joint2D::_bind_methods() {


	ObjectTypeDB::bind_method( _MD("set_node_a","node"), &Joint2D::set_node_a );
	ObjectTypeDB::bind_method( _MD("get_node_a"), &Joint2D::get_node_a );

	ObjectTypeDB::bind_method( _MD("set_node_b","node"), &Joint2D::set_node_b );
	ObjectTypeDB::bind_method( _MD("get_node_b"), &Joint2D::get_node_b );

	ObjectTypeDB::bind_method( _MD("set_bias","bias"), &Joint2D::set_bias );
	ObjectTypeDB::bind_method( _MD("get_bias"), &Joint2D::get_bias );

	ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "node_a"), _SCS("set_node_a"),_SCS("get_node_a") );
	ADD_PROPERTY( PropertyInfo( Variant::NODE_PATH, "node_b"), _SCS("set_node_b"),_SCS("get_node_b") );
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "bias/bias",PROPERTY_HINT_RANGE,"0,0.9,0.01"), _SCS("set_bias"),_SCS("get_bias") );

}



Joint2D::Joint2D() {

	bias=0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void PinJoint2D::_notification(int p_what) {

	switch(p_what) {
		case NOTIFICATION_DRAW: {
			if (is_inside_tree() && get_tree()->is_editor_hint()) {

				draw_line(Point2(-10,0),Point2(+10,0),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(0,-10),Point2(0,+10),Color(0.7,0.6,0.0,0.5),3);
			}
		} break;
	}

}

RID PinJoint2D::_configure_joint() {

	Node *node_a = has_node( get_node_a() ) ? get_node( get_node_a() ) : (Node*)NULL;
	Node *node_b = has_node( get_node_b() ) ? get_node( get_node_b() ) : (Node*)NULL;

	if (!node_a && !node_b)
		return RID();

	PhysicsBody2D *body_a=node_a ? node_a->cast_to<PhysicsBody2D>() : (PhysicsBody2D*)NULL;
	PhysicsBody2D *body_b=node_b ? node_b->cast_to<PhysicsBody2D>() : (PhysicsBody2D*)NULL;

	if (!body_a && !body_b)
		return RID();

	if (!body_a) {
		SWAP(body_a,body_b);
	} else if (body_b) {
		//add a collision exception between both
		Physics2DServer::get_singleton()->body_add_collision_exception(body_a->get_rid(),body_b->get_rid());
	}

	return Physics2DServer::get_singleton()->pin_joint_create(get_global_transform().get_origin(),body_a->get_rid(),body_b?body_b->get_rid():RID());

}


PinJoint2D::PinJoint2D() {


}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


void GrooveJoint2D::_notification(int p_what) {

	switch(p_what) {
		case NOTIFICATION_DRAW: {
			if (is_inside_tree() && get_tree()->is_editor_hint()) {

				draw_line(Point2(-10,0),Point2(+10,0),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(-10,length),Point2(+10,length),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(0,0),Point2(0,length),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(-10,initial_offset),Point2(+10,initial_offset),Color(0.8,0.8,0.9,0.5),5);
			}
		} break;
	}
}

RID GrooveJoint2D::_configure_joint(){


	Node *node_a = has_node( get_node_a() ) ? get_node( get_node_a() ) : (Node*)NULL;
	Node *node_b = has_node( get_node_b() ) ? get_node( get_node_b() ) : (Node*)NULL;

	if (!node_a || !node_b)
		return RID();

	PhysicsBody2D *body_a=node_a->cast_to<PhysicsBody2D>();
	PhysicsBody2D *body_b=node_b->cast_to<PhysicsBody2D>();

	if (!body_a || !body_b)
		return RID();

	Physics2DServer::get_singleton()->body_add_collision_exception(body_a->get_rid(),body_b->get_rid());

	Transform2D gt = get_global_transform();
	Vector2 groove_A1 = gt.get_origin();
	Vector2 groove_A2 = gt.xform( Vector2(0,length) );
	Vector2 anchor_B = gt.xform( Vector2(0,initial_offset) );


	return Physics2DServer::get_singleton()->groove_joint_create(groove_A1,groove_A2,anchor_B,body_a->get_rid(),body_b->get_rid());
}


void GrooveJoint2D::set_length(real_t p_length) {

	length=p_length;
	update();
}

real_t GrooveJoint2D::get_length() const {

	return length;
}


void GrooveJoint2D::set_initial_offset(real_t p_initial_offset) {

	initial_offset=p_initial_offset;
	update();
}

real_t GrooveJoint2D::get_initial_offset() const {

	return initial_offset;
}



void GrooveJoint2D::_bind_methods() {


	ObjectTypeDB::bind_method(_MD("set_length","length"),&GrooveJoint2D::set_length);
	ObjectTypeDB::bind_method(_MD("get_length"),&GrooveJoint2D::get_length);
	ObjectTypeDB::bind_method(_MD("set_initial_offset","offset"),&GrooveJoint2D::set_initial_offset);
	ObjectTypeDB::bind_method(_MD("get_initial_offset"),&GrooveJoint2D::get_initial_offset);

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "length", PROPERTY_HINT_EXP_RANGE,"1,65535,1"), _SCS("set_length"),_SCS("get_length"));
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "initial_offset", PROPERTY_HINT_EXP_RANGE,"1,65535,1"), _SCS("set_initial_offset"),_SCS("get_initial_offset"));
}

GrooveJoint2D::GrooveJoint2D() {

	length=50;
	initial_offset=25;
}




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////



void DampedSpringJoint2D::_notification(int p_what) {

	switch(p_what) {
		case NOTIFICATION_DRAW: {
			if (is_inside_tree() && get_tree()->is_editor_hint()) {

				draw_line(Point2(-10,0),Point2(+10,0),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(-10,length),Point2(+10,length),Color(0.7,0.6,0.0,0.5),3);
				draw_line(Point2(0,0),Point2(0,length),Color(0.7,0.6,0.0,0.5),3);
			}
		} break;
	}
}

RID DampedSpringJoint2D::_configure_joint(){


	Node *node_a = has_node( get_node_a() ) ? get_node( get_node_a() ) : (Node*)NULL;
	Node *node_b = has_node( get_node_b() ) ? get_node( get_node_b() ) : (Node*)NULL;

	if (!node_a || !node_b)
		return RID();

	PhysicsBody2D *body_a=node_a->cast_to<PhysicsBody2D>();
	PhysicsBody2D *body_b=node_b->cast_to<PhysicsBody2D>();

	if (!body_a || !body_b)
		return RID();

	Physics2DServer::get_singleton()->body_add_collision_exception(body_a->get_rid(),body_b->get_rid());

	Transform2D gt = get_global_transform();
	Vector2 anchor_A = gt.get_origin();
	Vector2 anchor_B = gt.xform( Vector2(0,length) );

	RID dsj = Physics2DServer::get_singleton()->damped_spring_joint_create(anchor_A,anchor_B,body_a->get_rid(),body_b->get_rid());
	if (rest_length)
		Physics2DServer::get_singleton()->damped_string_joint_set_param(dsj,Physics2DServer::DAMPED_STRING_REST_LENGTH,rest_length);
	Physics2DServer::get_singleton()->damped_string_joint_set_param(dsj,Physics2DServer::DAMPED_STRING_STIFFNESS,stiffness);
	Physics2DServer::get_singleton()->damped_string_joint_set_param(dsj,Physics2DServer::DAMPED_STRING_DAMPING,damping);

	return dsj;
}


void DampedSpringJoint2D::set_length(real_t p_length) {

	length=p_length;
	update();
}

real_t DampedSpringJoint2D::get_length() const {

	return length;
}

void DampedSpringJoint2D::set_rest_length(real_t p_rest_length) {

	rest_length=p_rest_length;
	update();
	if (get_joint().is_valid())
		Physics2DServer::get_singleton()->damped_string_joint_set_param(get_joint(),Physics2DServer::DAMPED_STRING_REST_LENGTH,p_rest_length?p_rest_length:length);

}

real_t DampedSpringJoint2D::get_rest_length() const {

	return rest_length;
}

void DampedSpringJoint2D::set_stiffness(real_t p_stiffness) {

	stiffness=p_stiffness;
	update();
	if (get_joint().is_valid())
		Physics2DServer::get_singleton()->damped_string_joint_set_param(get_joint(),Physics2DServer::DAMPED_STRING_STIFFNESS,p_stiffness);
}

real_t DampedSpringJoint2D::get_stiffness() const {

	return stiffness;
}

void DampedSpringJoint2D::set_damping(real_t p_damping) {

	damping=p_damping;
	update();
	if (get_joint().is_valid())
		Physics2DServer::get_singleton()->damped_string_joint_set_param(get_joint(),Physics2DServer::DAMPED_STRING_DAMPING,p_damping);
}

real_t DampedSpringJoint2D::get_damping() const {

	return damping;
}


void DampedSpringJoint2D::_bind_methods() {


	ObjectTypeDB::bind_method(_MD("set_length","length"),&DampedSpringJoint2D::set_length);
	ObjectTypeDB::bind_method(_MD("get_length"),&DampedSpringJoint2D::get_length);
	ObjectTypeDB::bind_method(_MD("set_rest_length","rest_length"),&DampedSpringJoint2D::set_rest_length);
	ObjectTypeDB::bind_method(_MD("get_rest_length"),&DampedSpringJoint2D::get_rest_length);
	ObjectTypeDB::bind_method(_MD("set_stiffness","stiffness"),&DampedSpringJoint2D::set_stiffness);
	ObjectTypeDB::bind_method(_MD("get_stiffness"),&DampedSpringJoint2D::get_stiffness);
	ObjectTypeDB::bind_method(_MD("set_damping","damping"),&DampedSpringJoint2D::set_damping);
	ObjectTypeDB::bind_method(_MD("get_damping"),&DampedSpringJoint2D::get_damping);

	ADD_PROPERTY( PropertyInfo( Variant::REAL, "length", PROPERTY_HINT_EXP_RANGE,"1,65535,1"), _SCS("set_length"),_SCS("get_length"));
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "rest_length", PROPERTY_HINT_EXP_RANGE,"0,65535,1"), _SCS("set_rest_length"),_SCS("get_rest_length"));
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "stiffness", PROPERTY_HINT_EXP_RANGE,"0.1,64,0.1"), _SCS("set_stiffness"),_SCS("get_stiffness"));
	ADD_PROPERTY( PropertyInfo( Variant::REAL, "damping", PROPERTY_HINT_EXP_RANGE,"0.01,16,0.01"), _SCS("set_damping"),_SCS("get_damping"));

}

DampedSpringJoint2D::DampedSpringJoint2D() {

	length=50;
	rest_length=0;
	stiffness=20;
	damping=1;
}
