/*************************************************************************/
/*  body_2d_sw.h                                                         */
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
#ifndef BODY_2D_SW_H
#define BODY_2D_SW_H

#include "collision_object_2d_sw.h"
#include "vset.h"
#include "area_2d_sw.h"

class Constraint2DSW;


class Body2DSW : public CollisionObject2DSW {


	Physics2DServer::BodyMode mode;

	Vector2 biased_linear_velocity;
	real_t biased_angular_velocity;

	Vector2 linear_velocity;
	real_t angular_velocity;

	real_t mass;
	real_t bounce;
	real_t friction;

	real_t _inv_mass;
	real_t _inv_inertia;

	Vector2 gravity;
	real_t density;

	real_t still_time;

	Vector2 applied_force;
	real_t applied_torque;


	SelfList<Body2DSW> active_list;
	SelfList<Body2DSW> inertia_update_list;
	SelfList<Body2DSW> direct_state_query_list;

	VSet<RID> exceptions;
	Physics2DServer::CCDMode continuous_cd_mode;
	bool omit_force_integration;
	bool active;
	bool can_sleep;
	bool first_time_kinematic;
	void _update_inertia();
	virtual void _shapes_changed();
	Transform2D new_transform;


	Map<Constraint2DSW*,int> constraint_map;

	struct AreaCMP {

		Area2DSW *area;
		_FORCE_INLINE_ bool operator<(const AreaCMP& p_cmp) const { return area->get_self() < p_cmp.area->get_self() ; }
		_FORCE_INLINE_ AreaCMP() {}
		_FORCE_INLINE_ AreaCMP(Area2DSW *p_area) { area=p_area;}
	};


	VSet<AreaCMP> areas;

	struct Contact {


		Vector2 local_pos;
		Vector2 local_normal;
		float depth;
		int local_shape;
		Vector2 collider_pos;
		int collider_shape;
		ObjectID collider_instance_id;
		RID collider;
		Vector2 collider_velocity_at_pos;
	};

	Vector<Contact> contacts; //no contacts by default
	int contact_count;

	struct ForceIntegrationCallback {

		ObjectID id;
		StringName method;
		Variant callback_udata;
	};

	ForceIntegrationCallback *fi_callback;


	uint64_t island_step;
	Body2DSW *island_next;
	Body2DSW *island_list_next;

	_FORCE_INLINE_ void _compute_area_gravity(const Area2DSW *p_area);

friend class Physics2DDirectBodyStateSW; // i give up, too many functions to expose

public:


	void set_force_integration_callback(ObjectID p_id, const StringName& p_method, const Variant &p_udata=Variant());


	_FORCE_INLINE_ void add_area(Area2DSW *p_area) { areas.insert(AreaCMP(p_area)); }
	_FORCE_INLINE_ void remove_area(Area2DSW *p_area) { areas.erase(AreaCMP(p_area)); }

	_FORCE_INLINE_ void set_max_contacts_reported(int p_size) { contacts.resize(p_size); contact_count=0; if (mode==Physics2DServer::BODY_MODE_KINEMATIC && p_size) set_active(true);}

	_FORCE_INLINE_ int get_max_contacts_reported() const { return contacts.size(); }

	_FORCE_INLINE_ bool can_report_contacts() const { return !contacts.empty(); }
	_FORCE_INLINE_ void add_contact(const Vector2& p_local_pos,const Vector2& p_local_normal, float p_depth, int p_local_shape, const Vector2& p_collider_pos, int p_collider_shape, ObjectID p_collider_instance_id, const RID& p_collider,const Vector2& p_collider_velocity_at_pos);


	_FORCE_INLINE_ void add_exception(const RID& p_exception) { exceptions.insert(p_exception);}
	_FORCE_INLINE_ void remove_exception(const RID& p_exception) { exceptions.erase(p_exception);}
	_FORCE_INLINE_ bool has_exception(const RID& p_exception) const { return exceptions.has(p_exception);}
	_FORCE_INLINE_ const VSet<RID>& get_exceptions() const { return exceptions;}

	_FORCE_INLINE_ uint64_t get_island_step() const { return island_step; }
	_FORCE_INLINE_ void set_island_step(uint64_t p_step) { island_step=p_step; }

	_FORCE_INLINE_ Body2DSW* get_island_next() const { return island_next; }
	_FORCE_INLINE_ void set_island_next(Body2DSW* p_next) { island_next=p_next; }

	_FORCE_INLINE_ Body2DSW* get_island_list_next() const { return island_list_next; }
	_FORCE_INLINE_ void set_island_list_next(Body2DSW* p_next) { island_list_next=p_next; }

	_FORCE_INLINE_ void add_constraint(Constraint2DSW* p_constraint, int p_pos) { constraint_map[p_constraint]=p_pos; }
	_FORCE_INLINE_ void remove_constraint(Constraint2DSW* p_constraint) { constraint_map.erase(p_constraint); }
	const Map<Constraint2DSW*,int>& get_constraint_map() const { return constraint_map; }

	_FORCE_INLINE_ void set_omit_force_integration(bool p_omit_force_integration) { omit_force_integration=p_omit_force_integration; }
	_FORCE_INLINE_ bool get_omit_force_integration() const { return omit_force_integration; }

	_FORCE_INLINE_ void set_linear_velocity(const Vector2& p_velocity) {linear_velocity=p_velocity; }
	_FORCE_INLINE_ Vector2 get_linear_velocity() const { return linear_velocity; }

	_FORCE_INLINE_ void set_angular_velocity(real_t p_velocity) { angular_velocity=p_velocity; }
	_FORCE_INLINE_ real_t get_angular_velocity() const { return angular_velocity; }

	_FORCE_INLINE_ void set_biased_linear_velocity(const Vector2& p_velocity) {biased_linear_velocity=p_velocity; }
	_FORCE_INLINE_ Vector2 get_biased_linear_velocity() const { return biased_linear_velocity; }

	_FORCE_INLINE_ void set_biased_angular_velocity(real_t p_velocity) { biased_angular_velocity=p_velocity; }
	_FORCE_INLINE_ real_t get_biased_angular_velocity() const { return biased_angular_velocity; }


	_FORCE_INLINE_ void apply_impulse(const Vector2& p_pos, const Vector2& p_j) {

		linear_velocity += p_j * _inv_mass;
		angular_velocity += _inv_inertia * p_pos.cross(p_j);
	}

	_FORCE_INLINE_ void apply_bias_impulse(const Vector2& p_pos, const Vector2& p_j) {

		biased_linear_velocity += p_j * _inv_mass;
		biased_angular_velocity += _inv_inertia * p_pos.cross(p_j);
	}

	void set_active(bool p_active);
	_FORCE_INLINE_ bool is_active() const { return active; }

	void set_param(Physics2DServer::BodyParameter p_param, float);
	float get_param(Physics2DServer::BodyParameter p_param) const;

	void set_mode(Physics2DServer::BodyMode p_mode);
	Physics2DServer::BodyMode get_mode() const;

	void set_state(Physics2DServer::BodyState p_state, const Variant& p_variant);
	Variant get_state(Physics2DServer::BodyState p_state) const;

	void set_applied_force(const Vector2& p_force) { applied_force=p_force; }
	Vector2 get_applied_force() const { return applied_force; }

	void set_applied_torque(real_t p_torque) { applied_torque=p_torque; }
	real_t get_applied_torque() const { return applied_torque; }


	_FORCE_INLINE_ void set_continuous_collision_detection_mode(Physics2DServer::CCDMode p_mode) { continuous_cd_mode=p_mode; }
	_FORCE_INLINE_ Physics2DServer::CCDMode get_continuous_collision_detection_mode() const { return continuous_cd_mode; }

	void set_space(Space2DSW *p_space);

	void update_inertias();

	_FORCE_INLINE_ real_t get_inv_mass() const { return _inv_mass; }
	_FORCE_INLINE_ real_t get_inv_inertia() const { return _inv_inertia; }
	_FORCE_INLINE_ real_t get_friction() const { return friction; }
	_FORCE_INLINE_ Vector2 get_gravity() const { return gravity; }
	_FORCE_INLINE_ real_t get_density() const { return density; }
	_FORCE_INLINE_ real_t get_bounce() const { return bounce; }

	void integrate_forces(real_t p_step);
	void integrate_velocities(real_t p_step);

	_FORCE_INLINE_ Vector2 get_motion() const {

		if (mode>Physics2DServer::BODY_MODE_KINEMATIC) {
			return new_transform.get_origin() - get_transform().get_origin();
		} else if (mode==Physics2DServer::BODY_MODE_KINEMATIC) {
			return  get_transform().get_origin() -new_transform.get_origin(); //kinematic simulates forward
		}
		return Vector2();
	}

	void call_queries();
	void wakeup_neighbours();

	bool sleep_test(real_t p_step);

	Body2DSW();
	~Body2DSW();

};


//add contact inline

void Body2DSW::add_contact(const Vector2& p_local_pos,const Vector2& p_local_normal, float p_depth, int p_local_shape, const Vector2& p_collider_pos, int p_collider_shape, ObjectID p_collider_instance_id, const RID& p_collider,const Vector2& p_collider_velocity_at_pos) {

	int c_max=contacts.size();

	if (c_max==0)
		return;

	Contact *c = &contacts[0];


	int idx=-1;

	if (contact_count<c_max) {
		idx=contact_count++;
	} else {

		float least_depth=1e20;
		int least_deep=-1;
		for(int i=0;i<c_max;i++) {

			if (i==0 || c[i].depth<least_depth) {
				least_deep=i;
				least_depth=c[i].depth;
			}
		}

		if (least_deep>=0 && least_depth<p_depth) {

			idx=least_deep;
		}
		if (idx==-1)
			return; //none least deepe than this
	}

	c[idx].local_pos=p_local_pos;
	c[idx].local_normal=p_local_normal;
	c[idx].depth=p_depth;
	c[idx].local_shape=p_local_shape;
	c[idx].collider_pos=p_collider_pos;
	c[idx].collider_shape=p_collider_shape;
	c[idx].collider_instance_id=p_collider_instance_id;
	c[idx].collider=p_collider;
	c[idx].collider_velocity_at_pos=p_collider_velocity_at_pos;

}


class Physics2DDirectBodyStateSW : public Physics2DDirectBodyState {

	OBJ_TYPE( Physics2DDirectBodyStateSW, Physics2DDirectBodyState );

public:

	static Physics2DDirectBodyStateSW *singleton;
	Body2DSW *body;
	real_t step;

	virtual Vector2 get_total_gravity() const {  return body->get_gravity();  } // get gravity vector working on this body space/area
	virtual float get_total_density() const {  return body->get_density();  } // get density of this body space/area

	virtual float get_inverse_mass() const {  return body->get_inv_mass();  } // get the mass
	virtual real_t get_inverse_inertia() const { return body->get_inv_inertia();   } // get density of this body space

	virtual void set_linear_velocity(const Vector2& p_velocity) {  body->set_linear_velocity(p_velocity);  }
	virtual Vector2 get_linear_velocity() const {  return body->get_linear_velocity();  }

	virtual void set_angular_velocity(real_t p_velocity) {  body->set_angular_velocity(p_velocity);  }
	virtual real_t get_angular_velocity() const {  return body->get_angular_velocity();  }

	virtual void set_transform(const Transform2D& p_transform) {  body->set_state(Physics2DServer::BODY_STATE_TRANSFORM,p_transform);  }
	virtual Transform2D get_transform() const {  return body->get_transform();  }

	virtual void set_sleep_state(bool p_enable) {  body->set_active(!p_enable);  }
	virtual bool is_sleeping() const {  return !body->is_active();  }

	virtual int get_contact_count() const {  return body->contact_count;  }

	virtual Vector2 get_contact_local_pos(int p_contact_idx) const {
		ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,Vector2());
		return body->contacts[p_contact_idx].local_pos;
	}
	virtual Vector2 get_contact_local_normal(int p_contact_idx) const { ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,Vector2());   return body->contacts[p_contact_idx].local_normal;   }
	virtual int get_contact_local_shape(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,-1);   return body->contacts[p_contact_idx].local_shape;   }

	virtual RID get_contact_collider(int p_contact_idx) const { ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,RID());   return body->contacts[p_contact_idx].collider;   }
	virtual Vector2 get_contact_collider_pos(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,Vector2());   return body->contacts[p_contact_idx].collider_pos;  }
	virtual ObjectID get_contact_collider_id(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,0);   return body->contacts[p_contact_idx].collider_instance_id;   }
	virtual int get_contact_collider_shape(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,0); return body->contacts[p_contact_idx].collider_shape;  }
	virtual Variant get_contact_collider_shape_metadata(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,Variant()); return body->get_shape_metadata(body->contacts[p_contact_idx].collider_shape);  }

	virtual Vector2 get_contact_collider_velocity_at_pos(int p_contact_idx) const {  ERR_FAIL_INDEX_V(p_contact_idx,body->contact_count,Vector2()); return body->contacts[p_contact_idx].collider_velocity_at_pos;  }

	virtual Physics2DDirectSpaceState* get_space_state();


	virtual real_t get_step() const { return step; }
	Physics2DDirectBodyStateSW() { singleton=this; body=NULL; }
};


#endif // BODY_2D_SW_H
