/*************************************************************************/
/*  variant.cpp                                                          */
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
#include "variant.h"
#include "resource.h"
#include "print_string.h"
#include "scene/main/node.h"
#include "scene/gui/control.h"
#include "io/marshalls.h"




String Variant::get_type_name(Variant::Type p_type) {
	
	switch( p_type ) {
		case NIL: {

			return "Nil";
		} break;

			// atomic types
		case BOOL: {

			return "bool";
		} break;
		case INT: {

			return "int";

		} break;
		case REAL: {

			return "float";

		} break;
		case STRING: {

			return "String";
		} break;

			// math types

		case VECTOR2: {

			return "Vector2";
		} break;
		case RECT2: {

			return "Rect2";
		} break;
		case MATRIX32: {

			return "Transform2D";
		} break;
		case VECTOR3: {

			return "Vector3";
		} break;
		case PLANE: {

			return "Plane";

		} break;
	/*
			case QUAT: {


			} break;*/
		case _AABB: {

			return "AABB";
		} break;
		case QUAT: {

			return "Quat";

		} break;
		case MATRIX3: {

			return "Basis";

		} break;
		case TRANSFORM: {

			return "Transform3D";

		} break;

			// misc types
		case COLOR: {

			return "Color";

		} break;
		case IMAGE: {

			return "Image";

		} break;
		case _RID: {

			return "RID";
		} break;
		case OBJECT: {

			return "Object";
		} break;
		case NODE_PATH: {

			return "NodePath";

		} break;
		case INPUT_EVENT: {

			return "InputEvent";

		} break;
		case DICTIONARY: {

			return "Dictionary";

		} break;
		case ARRAY: {

			return "Array";

		} break;

			// arrays
		case RAW_ARRAY: {

			return "PackedByteArray";

		} break;
		case INT_ARRAY: {

			return "PackedIntArray";

		} break;
		case REAL_ARRAY: {

			return "PackedFloatArray";

		} break;
		case STRING_ARRAY: {

			return "PackedStringArray";
		} break;
		case VECTOR2_ARRAY: {

			return "PackedVector2Array";

		} break;
		case VECTOR3_ARRAY: {

			return "PackedVector3Array";

		} break;
		case COLOR_ARRAY: {

			return "PackedColorArray";

		} break;
		default: {}
		}

	return "";
}


bool Variant::can_convert(Variant::Type p_type_from,Variant::Type p_type_to) {

	if (p_type_from==p_type_to)
		return true;
	if (p_type_to==NIL && p_type_from!=NIL) //nil can convert to anything
		return true;

	if (p_type_from == NIL) {
		return (p_type_to == OBJECT);
	};

	const Type *valid_types=NULL;
	const Type *invalid_types=NULL;

	switch(p_type_to) {
		case BOOL: {

			static const Type valid[]={
				INT,
				REAL,
				STRING,
				NIL,
			};

			valid_types=valid;
		} break;
		case INT: {

			static const Type valid[]={
				BOOL,
				REAL,
				STRING,
				NIL,
			};

			valid_types=valid;

		} break;
		case REAL: {

			static const Type valid[]={
				BOOL,
				INT,
				STRING,
				NIL,
			};

			valid_types=valid;

		} break;
		case STRING: {


			static const Type invalid[]={
				OBJECT,
				IMAGE,
				NIL
			};

			invalid_types=invalid;
		} break;
		case MATRIX32: {


			static const Type invalid[]={
				TRANSFORM,
				NIL
			};

			invalid_types=invalid;
		} break;
		case QUAT: {

			static const Type valid[]={
				MATRIX3,
				NIL
			};

			valid_types=valid;

		} break;
		case MATRIX3: {

			static const Type valid[]={
				QUAT,
				NIL
			};

			valid_types=valid;


		} break;
		case TRANSFORM: {

			static const Type valid[]={
				MATRIX32,
				QUAT,
				MATRIX3,
				NIL
			};

			valid_types=valid;

		} break;		

		case COLOR: {

			static const Type valid[] = {
				STRING,
				INT,
				NIL,
			};

			valid_types = valid;

		} break;

		case _RID: {

			static const Type valid[]={
				OBJECT,
				NIL
			};

			valid_types=valid;
		} break;
		case OBJECT: {

			static const Type valid[]={
				NIL
			};

			valid_types=valid;
		} break;
		case NODE_PATH: {

			static const Type valid[]={
				STRING,
				NIL
			};

			valid_types=valid;
		} break;
		case ARRAY: {


			static const Type valid[]={
				RAW_ARRAY,
				INT_ARRAY,
				STRING_ARRAY,
				REAL_ARRAY,
				COLOR_ARRAY,
				VECTOR2_ARRAY,
				VECTOR3_ARRAY,
				NIL
			};

			valid_types=valid;
		} break;
			// arrays
		case RAW_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};

			valid_types=valid;
		} break;
		case INT_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};
			valid_types=valid;
		} break;
		case REAL_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};

			valid_types=valid;
		} break;
		case STRING_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};
			valid_types=valid;
		} break;
		case VECTOR2_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};
			valid_types=valid;

		} break;
		case VECTOR3_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};
			valid_types=valid;

		} break;
		case COLOR_ARRAY: {

			static const Type valid[]={
				ARRAY,
				NIL
			};

			valid_types=valid;

		} break;
		default: {}
	}


	if (valid_types) {

		int i=0;
		while(valid_types[i]!=NIL) {

			if (p_type_from==valid_types[i])
				return true;
			i++;
		}
	} else if (invalid_types) {


		int i=0;
		while(invalid_types[i]!=NIL) {

			if (p_type_from==invalid_types[i])
				return false;
			i++;
		}
	}

	return false;

}

bool Variant::operator==(const Variant& p_variant) const {

	if (type!=p_variant.type) //evaluation of operator== needs to be more strict
		return false;
	bool v;
	Variant r;
	evaluate(OP_EQUAL,*this,p_variant,r,v);
	return r;

}

bool Variant::is_zero() const {

	switch( type ) {
		case NIL: {

			return true;
		} break;

		// atomic types
		case BOOL: {

			return _data._bool==false;
		} break;
		case INT: {

			return _data._int==0;

		} break;
		case REAL: {

			return _data._real==0;

		} break;
		case STRING: {

			return *reinterpret_cast<const String*>(_data._mem)==String();

		} break;

		// math types

		case VECTOR2: {

			return *reinterpret_cast<const Vector2*>(_data._mem)==Vector2();

		} break;
		case RECT2: {

			return *reinterpret_cast<const Rect2*>(_data._mem)==Rect2();

		} break;
		case MATRIX32: {

			return *_data._matrix32==Transform2D();

		} break;
		case VECTOR3: {

			return *reinterpret_cast<const Vector3*>(_data._mem)==Vector3();

		} break;
		case PLANE: {

			return *reinterpret_cast<const Plane*>(_data._mem)==Plane();

		} break;
/*
		case QUAT: {


		} break;*/
		case _AABB: {

			return *_data._aabb==AABB();
		} break;
		case QUAT: {

			*reinterpret_cast<const Quat*>(_data._mem)==Quat();

		} break;
		case MATRIX3: {

			return *_data._matrix3==Basis();

		} break;
		case TRANSFORM: {

			return *_data._transform == Transform3D();

		} break;

		// misc types
		case COLOR: {

			return *reinterpret_cast<const Color*>(_data._mem)==Color();

		} break;
		case IMAGE: {

			return _data._image->empty();

		} break;
		case _RID: {

			return *reinterpret_cast<const RID*>(_data._mem)==RID();
		} break;
		case OBJECT: {

			return _get_obj().obj==NULL;
		} break;
		case NODE_PATH: {

			return reinterpret_cast<const NodePath*>(_data._mem)->is_empty();

		} break;
		case INPUT_EVENT: {

			return _data._input_event->type==InputEvent::NONE;

		} break;
		case DICTIONARY: {

			return reinterpret_cast<const Dictionary*>(_data._mem)->empty();

		} break;
		case ARRAY: {

			return reinterpret_cast<const Array*>(_data._mem)->empty();

		} break;

		// arrays
		case RAW_ARRAY: {

			return reinterpret_cast<const DVector<uint8_t>*>(_data._mem)->size()==0;

		} break;
		case INT_ARRAY: {

			return reinterpret_cast<const DVector<int>*>(_data._mem)->size()==0;

		} break;
		case REAL_ARRAY: {

			return reinterpret_cast<const DVector<real_t>*>(_data._mem)->size()==0;

		} break;
		case STRING_ARRAY: {

			return reinterpret_cast<const DVector<String>*>(_data._mem)->size()==0;

		} break;
		case VECTOR2_ARRAY: {

			return reinterpret_cast<const DVector<Vector2>*>(_data._mem)->size()==0;

		} break;
		case VECTOR3_ARRAY: {

			return reinterpret_cast<const DVector<Vector3>*>(_data._mem)->size()==0;

		} break;
		case COLOR_ARRAY: {

			return reinterpret_cast<const DVector<Color>*>(_data._mem)->size()==0;

		} break;
		default: {}
	}

	return false;
}

void Variant::reference(const Variant& p_variant) {

	
	if (this == &p_variant)
		return;
		
	clear();
		
	type=p_variant.type;
		
	switch( p_variant.type ) {
		case NIL: {
		
			// none
		} break;
		
		// atomic types 		
		case BOOL: {
		
			_data._bool=p_variant._data._bool;
		} break;
		case INT: {
		
			_data._int=p_variant._data._int;
		
		} break;
		case REAL: {
		
			_data._real=p_variant._data._real;
		
		} break;
		case STRING: {
		
			memnew_placement( _data._mem, String( *reinterpret_cast<const String*>(p_variant._data._mem) ) );
			
		} break;
		
		// math types
		
		case VECTOR2: {
		
			memnew_placement( _data._mem, Vector2( *reinterpret_cast<const Vector2*>(p_variant._data._mem) ) );
		
		} break;
		case RECT2: {
		
			memnew_placement( _data._mem, Rect2( *reinterpret_cast<const Rect2*>(p_variant._data._mem) ) );
		
		} break;
		case MATRIX32: {

			_data._matrix32 = memnew( Transform2D( *p_variant._data._matrix32 ) );

		} break;
		case VECTOR3: {
		
			memnew_placement( _data._mem, Vector3( *reinterpret_cast<const Vector3*>(p_variant._data._mem) ) );
		
		} break;
		case PLANE: {
		
			memnew_placement( _data._mem, Plane( *reinterpret_cast<const Plane*>(p_variant._data._mem) ) );
		
		} break;
/*		
		case QUAT: {
		
		
		} break;*/
		case _AABB: {
		
			_data._aabb = memnew( AABB( *p_variant._data._aabb ) );
		} break;
		case QUAT: {
		
			memnew_placement( _data._mem, Quat( *reinterpret_cast<const Quat*>(p_variant._data._mem) ) );
		
		} break;
		case MATRIX3: {
		
			_data._matrix3 = memnew( Basis( *p_variant._data._matrix3 ) );
		
		} break;
		case TRANSFORM: {
		
			_data._transform = memnew( Transform3D( *p_variant._data._transform ) );
		
		} break;
		
		// misc types		
		case COLOR: {
		
			memnew_placement( _data._mem, Color( *reinterpret_cast<const Color*>(p_variant._data._mem) ) );
		
		} break;
		case IMAGE: {
		
			_data._image = memnew( Image( *p_variant._data._image ) );
		
		} break;
		case _RID: {
		
			memnew_placement( _data._mem, RID( *reinterpret_cast<const RID*>(p_variant._data._mem) ) );
		} break;
		case OBJECT: {
		
			memnew_placement( _data._mem, ObjData( p_variant._get_obj() ) );
		} break;
		case NODE_PATH: {
		
			memnew_placement( _data._mem, NodePath( *reinterpret_cast<const NodePath*>(p_variant._data._mem) ) );
		
		} break;
		case INPUT_EVENT: {
			
			_data._input_event= memnew( InputEvent( *p_variant._data._input_event ) );
			
		} break;
		case DICTIONARY: {
			
			memnew_placement( _data._mem, Dictionary( *reinterpret_cast<const Dictionary*>(p_variant._data._mem) ) );
			
		} break;
		case ARRAY: {
			
			memnew_placement( _data._mem, Array ( *reinterpret_cast<const Array*>(p_variant._data._mem) ) );
			
		} break;
		
		// arrays
		case RAW_ARRAY: {
		
			memnew_placement( _data._mem, DVector<uint8_t> ( *reinterpret_cast<const DVector<uint8_t>*>(p_variant._data._mem) ) );
		
		} break;
		case INT_ARRAY: {
		
			memnew_placement( _data._mem, DVector<int> ( *reinterpret_cast<const DVector<int>*>(p_variant._data._mem) ) );
		
		} break;
		case REAL_ARRAY: {
		
			memnew_placement( _data._mem, DVector<real_t> ( *reinterpret_cast<const DVector<real_t>*>(p_variant._data._mem) ) );
		
		} break;
		case STRING_ARRAY: {
		
			memnew_placement( _data._mem, DVector<String> ( *reinterpret_cast<const DVector<String>*>(p_variant._data._mem) ) );
		
		} break;
		case VECTOR2_ARRAY: {

			memnew_placement( _data._mem, DVector<Vector2> ( *reinterpret_cast<const DVector<Vector2>*>(p_variant._data._mem) ) );

		} break;
		case VECTOR3_ARRAY: {
		
			memnew_placement( _data._mem, DVector<Vector3> ( *reinterpret_cast<const DVector<Vector3>*>(p_variant._data._mem) ) );
		
		} break;
		case COLOR_ARRAY: {
		
			memnew_placement( _data._mem, DVector<Color> ( *reinterpret_cast<const DVector<Color>*>(p_variant._data._mem) ) );
		
		} break;
		default: {}
	}		


}
void Variant::clear() {

	switch(type) {
		case STRING: {
		
			reinterpret_cast<String*>(_data._mem)->~String();
		} break;
	/*
		// no point, they don't allocate memory
		VECTOR3,
		PLANE,
		QUAT,
		COLOR,
		VECTOR2,
		RECT2
	*/
		case MATRIX32: {

			memdelete( _data._matrix32 );

		} break;
		case _AABB: {
		
			memdelete( _data._aabb );
		
		} break;
		case MATRIX3: {
		
			memdelete( _data._matrix3 );
		} break;
		case TRANSFORM: {
		
			memdelete( _data._transform );
		
		} break;
		
		// misc types		
		case IMAGE: {
		
			memdelete( _data._image );
		
		} break;
		case NODE_PATH: {
		
			reinterpret_cast<NodePath*>(_data._mem)->~NodePath();
		
		} break;
		case OBJECT: {
		
			_get_obj().obj=NULL;
			_get_obj().ref.unref();
		} break;
		case _RID: {
			// not much need probably
			reinterpret_cast<RID*>(_data._mem)->~RID();
		} break;
		case DICTIONARY: {
			
			reinterpret_cast<Dictionary*>(_data._mem)->~Dictionary();
			
		} break;
		case ARRAY: {
			
			reinterpret_cast<Array*>(_data._mem)->~Array();
			
		} break;
		case INPUT_EVENT: {
			
			memdelete( _data._input_event );
			
		} break;
		
		// arrays
		case RAW_ARRAY: {
		
			reinterpret_cast< DVector<uint8_t>* >(_data._mem)->~DVector<uint8_t>();
		
		} break;
		case INT_ARRAY: {
		
			reinterpret_cast< DVector<int>* >(_data._mem)->~DVector<int>();
		
		} break;
		case REAL_ARRAY: {
		
			reinterpret_cast< DVector<real_t>* >(_data._mem)->~DVector<real_t>();
		
		} break;
		case STRING_ARRAY: {
		
			reinterpret_cast< DVector<String>* >(_data._mem)->~DVector<String>();
		
		} break;
		case VECTOR2_ARRAY: {

			reinterpret_cast< DVector<Vector2>* >(_data._mem)->~DVector<Vector2>();

		} break;
		case VECTOR3_ARRAY: {
		
			reinterpret_cast< DVector<Vector3>* >(_data._mem)->~DVector<Vector3>();
		
		} break;
		case COLOR_ARRAY: {
		
			reinterpret_cast< DVector<Color>* >(_data._mem)->~DVector<Color>();
		
		} break;
		default: {} /* not needed */
	}
	
	type=NIL;

}
	

Variant::operator signed int() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
Variant::operator unsigned int() const {

		switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}

Variant::operator int64_t() const {

	switch( type ) {

		case NIL: return 0;
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {

			return 0;
		}
	}

	return 0;
}

/*
Variant::operator long unsigned int() const {

	switch( type ) {

		case NIL: return 0;
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {

			return 0;
		}
	}

	return 0;
};
*/

Variant::operator uint64_t() const {

	switch( type ) {

		case NIL: return 0;
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {

			return 0;
		}
	}

	return 0;
}

#ifdef NEED_LONG_INT
Variant::operator signed long() const {

	switch( type ) {

		case NIL: return 0;
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {

			return 0;
		}
	}

	return 0;
};

Variant::operator unsigned long() const {

	switch( type ) {

		case NIL: return 0;
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {

			return 0;
		}
	}

	return 0;
};
#endif


Variant::operator signed short() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
Variant::operator unsigned short() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
Variant::operator signed char() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
Variant::operator unsigned char() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1 : 0;
		case INT: return _data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_int();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
#ifndef CHARTYPE_16BITS
Variant::operator CharType() const {

	return operator unsigned int();
}
#endif

Variant::operator float() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1.0 : 0.0;
		case INT: return (float)_data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_double();
		default: {
		
			return 0;
		}		
	}
	
	return 0;
}
Variant::operator double() const {

	switch( type ) {
	
		case NIL: return 0; 
		case BOOL: return _data._bool ? 1.0 : 0.0;
		case INT: return (float)_data._int;
		case REAL: return _data._real;
		case STRING: return operator String().to_double();
		default: {
		
			return 0;
		}		
	}
	
	return true;
}

Variant::operator StringName() const {
	
	if (type==NODE_PATH) {
		return reinterpret_cast<const NodePath*>(_data._mem)->get_sname();
	}
	return StringName(operator String());
}

struct _VariantStrPair {

	String key;
	String value;

	bool operator<(const _VariantStrPair& p) const {

		return key < p.key;
	}
};

Variant::operator String() const {

	switch( type ) {
	
		case NIL: return ""; 
		case BOOL: return _data._bool ? "True" : "False";
		case INT: return String::num(_data._int);
		case REAL: return String::num(_data._real);
		case STRING: return *reinterpret_cast<const String*>(_data._mem);
		case VECTOR2: return operator Vector2();
		case RECT2: return operator Rect2();
		case MATRIX32: return operator Transform2D();
		case VECTOR3: return operator Vector3();
		case PLANE: return operator Plane();
		//case QUAT: 
		case _AABB: return operator AABB();
		case QUAT: return operator Quat();
		case MATRIX3: return operator Basis();
		case TRANSFORM: return operator Transform3D();
		case NODE_PATH: return operator NodePath();
		case INPUT_EVENT: return operator InputEvent();
		case COLOR: return String::num( operator Color().r)+","+String::num( operator Color().g)+","+String::num( operator Color().b)+","+String::num( operator Color().a) ;
		case DICTIONARY: {
			
			const Dictionary &d =*reinterpret_cast<const Dictionary*>(_data._mem);
			//const String *K=NULL;
			String str;
			List<Variant> keys;
			d.get_key_list(&keys);

			Vector<_VariantStrPair> pairs;

			for(List<Variant>::Element *E=keys.front();E;E=E->next()) {

				_VariantStrPair sp;
				sp.key=String(E->get());
				sp.value=d[E->get()];
				pairs.push_back(sp);
			}

			pairs.sort();

			for(int i=0;i<pairs.size();i++) {
				if (i>0)
					str+=", ";
				str+="("+pairs[i].key+":"+pairs[i].value+")";
			}
			
			return str;
		} break;
		case VECTOR3_ARRAY: { 
		
			DVector<Vector3> vec = operator DVector<Vector3>();
			String str;
			for(int i=0;i<vec.size();i++) {
			
				if (i>0)
					str+=", ";
				str=str+Variant( vec[i] );
			}
			return str;
		} break;
		case STRING_ARRAY: {

			DVector<String> vec = operator DVector<String>();
			String str;
			for(int i=0;i<vec.size();i++) {

				if (i>0)
					str+=", ";
				str=str+vec[i];
			}
			return str;
		} break;
		case INT_ARRAY: {

			DVector<int> vec = operator DVector<int>();
			String str;
			for(int i=0;i<vec.size();i++) {

				if (i>0)
					str+=", ";
				str=str+itos(vec[i]);
			}
			return str;
		} break;
		case REAL_ARRAY: {

			DVector<real_t> vec = operator DVector<real_t>();
			String str;
			for(int i=0;i<vec.size();i++) {

				if (i>0)
					str+=", ";
				str=str+rtos(vec[i]);
			}
			return str;
		} break;
		case ARRAY: {

			Array arr = operator Array();
			String str;
			for (int i=0; i<arr.size(); i++) {
				if (i)
					str+=", ";
				str += String(arr[i]);
			};
			return str;

		} break;
		case OBJECT: {

			if (_get_obj().obj)
				return "["+_get_obj().obj->get_type()+":"+itos(_get_obj().obj->get_instance_ID())+"]";
			else
				return "[Object:null]";

		} break;
		default: {
			return "["+get_type_name(type)+"]";
		}		
	}
	
	return "";
}

Variant::operator Vector2() const {

	if (type==VECTOR2)
		return *reinterpret_cast<const Vector2*>(_data._mem);
	else if (type==VECTOR3)
		return Vector2(reinterpret_cast<const Vector3*>(_data._mem)->x,reinterpret_cast<const Vector3*>(_data._mem)->y);
	else
		return Vector2();

}
Variant::operator Rect2() const {

	if (type==RECT2)
		return *reinterpret_cast<const Rect2*>(_data._mem);
	else
		return Rect2();

}

Variant::operator Vector3() const {

	if (type==VECTOR3)
		return *reinterpret_cast<const Vector3*>(_data._mem);
	else
		return Vector3();

}
Variant::operator Plane() const {

	if (type==PLANE)
		return *reinterpret_cast<const Plane*>(_data._mem);
	else
		return Plane();

}
Variant::operator AABB() const {

	if (type==_AABB)
		return *_data._aabb;
	else
		return AABB();
}

Variant::operator Basis() const {

	if (type==MATRIX3)
		return *_data._matrix3;
	else if (type==QUAT)
		return *reinterpret_cast<const Quat*>(_data._mem);
	else if (type==TRANSFORM)
		return _data._transform->basis;
	else
		return Basis();
}

Variant::operator Quat() const {

	if (type==QUAT)
		return *reinterpret_cast<const Quat*>(_data._mem);
	else if (type==MATRIX3)
		return 	*_data._matrix3;
	else if (type==TRANSFORM)
		return 	_data._transform->basis;
	else
		return Quat();
}



Variant::operator Transform3D() const {

	if (type==TRANSFORM)
		return *_data._transform;
	else if (type==MATRIX3)
		return Transform3D(*_data._matrix3,Vector3());
	else if (type==QUAT)
		return Transform3D(Basis(*reinterpret_cast<const Quat*>(_data._mem)),Vector3());
	else
		return Transform3D();
}

 Variant::operator Transform2D() const {

	 if (type==MATRIX32) {
		 return *_data._matrix32;
	 } else if (type==TRANSFORM) {
		 const Transform3D& t = *_data._transform;;
		 Transform2D m;
		 m.elements[0][0]=t.basis.elements[0][0];
		 m.elements[0][1]=t.basis.elements[1][0];
		 m.elements[1][0]=t.basis.elements[0][1];
		 m.elements[1][1]=t.basis.elements[1][1];
		 m.elements[2][0]=t.origin[0];
		 m.elements[2][1]=t.origin[1];
		 return m;
	 } else
		 return Transform2D();
}


Variant::operator Color() const {

	if (type==COLOR)
		return *reinterpret_cast<const Color*>(_data._mem);
	else
		return Color();
}
Variant::operator Image() const {

	if (type==IMAGE)
		return *_data._image;
	else
		return Image();
}

Variant::operator NodePath() const {

	if (type==NODE_PATH)
		return *reinterpret_cast<const NodePath*>(_data._mem);
	else if (type==STRING)
		return NodePath(operator String());
	else
		return NodePath();
}


Variant::operator RefPtr() const {

	if (type==OBJECT)
		return _get_obj().ref;
	else
		return RefPtr();
}

Variant::operator RID() const {

	if (type==_RID)
		return *reinterpret_cast<const RID*>(_data._mem);
	else if (type==OBJECT && !_get_obj().ref.is_null()) {
		return _get_obj().ref.get_rid();
	} else
		return RID();
}

Variant::operator Object*() const {

	if (type==OBJECT)
		return _get_obj().obj;
	else
		return NULL;
}
Variant::operator Node*() const {

	if (type==OBJECT)
		return _get_obj().obj?_get_obj().obj->cast_to<Node>():NULL;
	else
		return NULL;
}
Variant::operator Control*() const {

	if (type==OBJECT)
		return _get_obj().obj?_get_obj().obj->cast_to<Control>():NULL;
	else
		return NULL;
}

Variant::operator InputEvent() const {
	
	if (type==INPUT_EVENT)
		return *reinterpret_cast<const InputEvent*>(_data._input_event);
	else
		return InputEvent();
}

Variant::operator Dictionary() const {
	
	if (type==DICTIONARY) 
		return *reinterpret_cast<const Dictionary*>(_data._mem);
	else
		return Dictionary();
}

template<class DA,class SA>
inline DA _convert_array(const SA& p_array) {

	DA da;
	da.resize(p_array.size());

	for(int i=0;i<p_array.size();i++) {

		da.set( i, Variant(p_array.get(i)) );
	}

	return da;
}

template<class DA>
inline DA _convert_array_from_variant(const Variant& p_variant) {

	switch(p_variant.get_type()) {


		case Variant::ARRAY: { return _convert_array<DA,Array >( p_variant.operator Array () ); }
		case Variant::RAW_ARRAY: { return _convert_array<DA,DVector<uint8_t> >( p_variant.operator DVector<uint8_t> () ); }
		case Variant::INT_ARRAY: { return _convert_array<DA,DVector<int> >( p_variant.operator DVector<int> () ); }
		case Variant::REAL_ARRAY: { return _convert_array<DA,DVector<real_t> >( p_variant.operator DVector<real_t> () ); }
		case Variant::STRING_ARRAY: { return _convert_array<DA,DVector<String> >( p_variant.operator DVector<String> () ); }
		case Variant::VECTOR2_ARRAY: { return _convert_array<DA,DVector<Vector2> >( p_variant.operator DVector<Vector2> () ); }
		case Variant::VECTOR3_ARRAY: { return _convert_array<DA,DVector<Vector3> >( p_variant.operator DVector<Vector3> () ); }
		case Variant::COLOR_ARRAY: { return _convert_array<DA,DVector<Color> >( p_variant.operator DVector<Color>() ); }
		default: { return DA(); }
	}

	return DA();
}

Variant::operator Array() const {
	
	if (type==ARRAY) 
		return *reinterpret_cast<const Array*>(_data._mem);
	else 
		return _convert_array_from_variant<Array >(*this);
}

Variant::operator DVector<uint8_t>() const {

	if (type==RAW_ARRAY)
		return *reinterpret_cast<const DVector<uint8_t>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<uint8_t> >(*this);
}
Variant::operator DVector<int>() const {

	if (type==INT_ARRAY)
		return *reinterpret_cast<const DVector<int>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<int> >(*this);

}
Variant::operator DVector<real_t>() const {

	if (type==REAL_ARRAY)
		return *reinterpret_cast<const DVector<real_t>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<real_t> >(*this);

}

Variant::operator DVector<String>() const {

	if (type==STRING_ARRAY)
		return *reinterpret_cast<const DVector<String>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<String> >(*this);


}
Variant::operator DVector<Vector3>() const {

	if (type==VECTOR3_ARRAY)
		return *reinterpret_cast<const DVector<Vector3>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<Vector3> >(*this);


}
Variant::operator DVector<Vector2>() const {

	if (type==VECTOR2_ARRAY)
		return *reinterpret_cast<const DVector<Vector2>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<Vector2> >(*this);


}

Variant::operator DVector<Color>() const {

	if (type==COLOR_ARRAY)
		return *reinterpret_cast<const  DVector<Color>* >(_data._mem);
	else
		return _convert_array_from_variant<DVector<Color> >(*this);

}

/* helpers */


Variant::operator Vector<RID>() const {

	Array va= operator Array();
	Vector<RID> rids;
	rids.resize(va.size());
	for(int i=0;i<rids.size();i++)
		rids[i]=va[i];
	return rids;
}

Variant::operator Vector<Vector2>() const {

	DVector<Vector2> from=operator DVector<Vector2>();
	Vector<Vector2> to;
	int len=from.size();
	if (len==0)
		return Vector<Vector2>();
	to.resize(len);
	DVector<Vector2>::Read r = from.read();
	Vector2 *w = &to[0];
	for (int i=0;i<len;i++) {

		w[i]=r[i];
	}
	return to;
}

Variant::operator DVector<Plane>() const {

	Array va= operator Array();
	DVector<Plane> planes;
	int va_size=va.size();
	if (va_size==0)
		return planes;

	planes.resize(va_size);
	DVector<Plane>::Write w = planes.write();

	for(int i=0;i<va_size;i++)
		w[i]=va[i];

	return planes;
}

Variant::operator DVector<Face3>() const {

	DVector<Vector3> va= operator DVector<Vector3>();
	DVector<Face3> faces;
	int va_size=va.size();
	if (va_size==0)
		return faces;

	faces.resize(va_size/3);
	DVector<Face3>::Write w = faces.write();
	DVector<Vector3>::Read r = va.read();

	for(int i=0;i<va_size;i++)
		w[i/3].vertex[i%3]=r[i];

	return faces;
}

Variant::operator Vector<Plane>() const {

	Array va= operator Array();
	Vector<Plane> planes;
	int va_size=va.size();
	if (va_size==0)
		return planes;

	planes.resize(va_size);

	for(int i=0;i<va_size;i++)
		planes[i]=va[i];

	return planes;
}

Variant::operator Vector<Variant>() const {
	
	Array from=operator Array();
	Vector<Variant> to;
	int len=from.size();
	to.resize(len);
	for (int i=0;i<len;i++) {
	
		to[i]=from[i];
	}
	return to;
	
}

Variant::operator Vector<uint8_t>() const {

	DVector<uint8_t> from=operator DVector<uint8_t>();
	Vector<uint8_t> to;
	int len=from.size();
	to.resize(len);
	for (int i=0;i<len;i++) {
	
		to[i]=from[i];
	}
	return to;
}
Variant::operator Vector<int>() const {

	DVector<int> from=operator DVector<int>();
	Vector<int> to;
	int len=from.size();
	to.resize(len);
	for (int i=0;i<len;i++) {
	
		to[i]=from[i];
	}
	return to;
}
Variant::operator Vector<real_t>() const {

	DVector<real_t> from=operator DVector<real_t>();
	Vector<real_t> to;
	int len=from.size();
	to.resize(len);
	for (int i=0;i<len;i++) {
	
		to[i]=from[i];
	}
	return to;
}

Variant::operator Vector<String>() const {

	DVector<String> from=operator DVector<String>();
	Vector<String> to;
	int len=from.size();
	to.resize(len);
	for (int i=0;i<len;i++) {
	
		to[i]=from[i];
	}
	return to;

}
Variant::operator Vector<Vector3>() const {

	DVector<Vector3> from=operator DVector<Vector3>();
	Vector<Vector3> to;
	int len=from.size();
	if (len==0)
		return Vector<Vector3>();
	to.resize(len);
	DVector<Vector3>::Read r = from.read();
	Vector3 *w = &to[0];
	for (int i=0;i<len;i++) {
	
		w[i]=r[i];
	}
	return to;

}
Variant::operator Vector<Color>() const {

	DVector<Color> from=operator DVector<Color>();
	Vector<Color> to;
	int len=from.size();
	if (len==0)
		return Vector<Color>();
	to.resize(len);
	DVector<Color>::Read r = from.read();
	Color *w = &to[0];
	for (int i=0;i<len;i++) {

		w[i]=r[i];
	}
	return to;
}

Variant::operator Margin() const {

	return (Margin)operator int();
}
Variant::operator Orientation() const {

	return (Orientation)operator int();
}

Variant::operator IP_Address() const {

	if (type==REAL_ARRAY || type==INT_ARRAY || type==RAW_ARRAY) {

		DVector<int> addr=operator DVector<int>();
		if (addr.size()==4) {
			return IP_Address(addr.get(0),addr.get(1),addr.get(2),addr.get(3));
		}
	}

	return IP_Address( operator String() );
}

Variant::Variant(bool p_bool) {

	type=BOOL;
	_data._bool=p_bool;
}

/*
Variant::Variant(long unsigned int p_long) {

	type=INT;
	_data._int=p_long;
};
*/

Variant::Variant(signed int p_int) {

	type=INT;
	_data._int=p_int;

}
Variant::Variant(unsigned int p_int) {

	type=INT;
	_data._int=p_int;

}

#ifdef NEED_LONG_INT

Variant::Variant(signed long p_int) {
	
	type=INT;
	_data._int=p_int;
	
}
Variant::Variant(unsigned long p_int) {
	
	type=INT;
	_data._int=p_int;
	
}
#endif

Variant::Variant(int64_t p_int) {

	type=INT;
	_data._int=p_int;

}

Variant::Variant(uint64_t p_int) {

	type=INT;
	_data._int=p_int;

}

Variant::Variant(signed short p_short) {

	type=INT;
	_data._int=p_short;

}
Variant::Variant(unsigned short p_short) {

	type=INT;
	_data._int=p_short;

}
Variant::Variant(signed char p_char) {

	type=INT;
	_data._int=p_char;

}
Variant::Variant(unsigned char p_char) {

	type=INT;
	_data._int=p_char;

}
Variant::Variant(float p_float) {

	type=REAL;
	_data._real=p_float;

}
Variant::Variant(double p_double) {

	type=REAL;
	_data._real=p_double;
}

Variant::Variant(const StringName& p_string) {
	
	type=STRING;
	memnew_placement( _data._mem, String( p_string.operator String() ) );
	
}
Variant::Variant(const String& p_string) {

	type=STRING;
	memnew_placement( _data._mem, String( p_string ) );

}

Variant::Variant(const char * const p_cstring) {

	type=STRING;
	memnew_placement( _data._mem, String( (const char*)p_cstring ) );

}

Variant::Variant(const CharType * p_wstring) {

	type=STRING;
	memnew_placement( _data._mem, String( p_wstring ) );

}
Variant::Variant(const Vector3& p_vector3) {

	type=VECTOR3;
	memnew_placement( _data._mem, Vector3( p_vector3 ) );

}
Variant::Variant(const Vector2& p_vector2) {

	type=VECTOR2;
	memnew_placement( _data._mem, Vector2( p_vector2 ) );

}
Variant::Variant(const Rect2& p_rect2) {

	type=RECT2;
	memnew_placement( _data._mem, Rect2( p_rect2 ) );

}

Variant::Variant(const Plane& p_plane) {

	type=PLANE;
	memnew_placement( _data._mem, Plane( p_plane ) );

}
Variant::Variant(const AABB& p_aabb) {

	type=_AABB;
	_data._aabb = memnew( AABB( p_aabb ) );
}

Variant::Variant(const Basis& p_matrix) {

	type=MATRIX3;
	_data._matrix3= memnew( Basis( p_matrix ) );

}

Variant::Variant(const Quat& p_quat) {

	type=QUAT;
	memnew_placement( _data._mem, Quat( p_quat ) );

}
Variant::Variant(const Transform3D& p_transform) {

	type=TRANSFORM;
	_data._transform = memnew( Transform3D( p_transform ) );

}

Variant::Variant(const Transform2D& p_transform) {

	type=MATRIX32;
	_data._matrix32 = memnew( Transform2D( p_transform ) );

}
Variant::Variant(const Color& p_color) {

	type=COLOR;
	memnew_placement( _data._mem, Color(p_color) );

}
Variant::Variant(const Image& p_image) {

	type=IMAGE;
	_data._image=memnew( Image(p_image) );

}

Variant::Variant(const NodePath& p_node_path) {

	type=NODE_PATH;
	memnew_placement( _data._mem, NodePath(p_node_path) );

}

Variant::Variant(const InputEvent& p_input_event) {
	
	type=INPUT_EVENT;
	_data._input_event = memnew( InputEvent(p_input_event) );	

}

Variant::Variant(const RefPtr& p_resource) {

	type=OBJECT;
	memnew_placement( _data._mem, ObjData );
	REF ref = p_resource;
	_get_obj().obj=ref.ptr();
	_get_obj().ref=p_resource;

}

Variant::Variant(const RID& p_rid) {

	type=_RID;
	memnew_placement( _data._mem, RID(p_rid) );

}

Variant::Variant(const Object* p_object) {

	type=OBJECT;

	memnew_placement( _data._mem, ObjData );
	_get_obj().obj=const_cast<Object*>(p_object);
}

Variant::Variant(const Dictionary& p_dictionary) {
	
	type=DICTIONARY;
	memnew_placement( _data._mem, (Dictionary)( p_dictionary) );
	
}

Variant::Variant(const Array& p_array) {

	type=ARRAY;
	memnew_placement( _data._mem, Array(p_array) );

}

Variant::Variant(const DVector<Plane>& p_array) {

	
	type=ARRAY;

	Array *plane_array=memnew_placement( _data._mem, Array );
	
	plane_array->resize( p_array.size() );
	
	for (int i=0;i<p_array.size();i++) {
	
		plane_array->operator [](i)=Variant(p_array[i]);
	}
}

Variant::Variant(const Vector<Plane>& p_array) {


	type=ARRAY;

	Array *plane_array=memnew_placement( _data._mem, Array );

	plane_array->resize( p_array.size() );

	for (int i=0;i<p_array.size();i++) {

		plane_array->operator [](i)=Variant(p_array[i]);
	}
}

Variant::Variant(const Vector<RID>& p_array) {


	type=ARRAY;

	Array *rid_array=memnew_placement( _data._mem, Array );

	rid_array->resize( p_array.size() );

	for (int i=0;i<p_array.size();i++) {

		rid_array->set(i,Variant(p_array[i]));
	}
}

Variant::Variant(const Vector<Vector2>& p_array) {


	type=NIL;
	DVector<Vector2> v;
	int len=p_array.size();
	if (len>0) {
		v.resize(len);
		DVector<Vector2>::Write w = v.write();
		const Vector2 *r = p_array.ptr();

		for (int i=0;i<len;i++)
			w[i]=r[i];
	}
	*this=v;
}


Variant::Variant(const DVector<uint8_t>& p_raw_array) {

	type=RAW_ARRAY;
	memnew_placement( _data._mem, DVector<uint8_t>(p_raw_array) );

}
Variant::Variant(const DVector<int>& p_int_array) {

	type=INT_ARRAY;
	memnew_placement( _data._mem, DVector<int>(p_int_array) );

}
Variant::Variant(const DVector<real_t>& p_real_array) {

	type=REAL_ARRAY;
	memnew_placement( _data._mem, DVector<real_t>(p_real_array) );

}
Variant::Variant(const DVector<String>& p_string_array) {

	type=STRING_ARRAY;
	memnew_placement( _data._mem, DVector<String>(p_string_array) );

}
Variant::Variant(const DVector<Vector3>& p_vector3_array) {

	type=VECTOR3_ARRAY;
	memnew_placement( _data._mem, DVector<Vector3>(p_vector3_array) );

}

Variant::Variant(const DVector<Vector2>& p_vector2_array) {

	type=VECTOR2_ARRAY;
	memnew_placement( _data._mem, DVector<Vector2>(p_vector2_array) );

}
Variant::Variant(const DVector<Color>& p_color_array) {

	type=COLOR_ARRAY;
	memnew_placement( _data._mem, DVector<Color>(p_color_array) );
}

Variant::Variant(const DVector<Face3>& p_face_array) {


	DVector<Vector3> vertices;
	int face_count=p_face_array.size();
	vertices.resize(face_count*3);

	if (face_count) {
		DVector<Face3>::Read r = p_face_array.read();
		DVector<Vector3>::Write w = vertices.write();

		for(int i=0;i<face_count;i++) {

			for(int j=0;j<3;j++)
				w[i*3+j]=r[i].vertex[j];
		}

		r=DVector<Face3>::Read();
		w=DVector<Vector3>::Write();

	}

	type = NIL;

	*this = vertices;
}

/* helpers */

Variant::Variant(const Vector<Variant>& p_array) {

	type=NIL;
	Array v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

Variant::Variant(const Vector<uint8_t>& p_array) {

	type=NIL;
	DVector<uint8_t> v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

Variant::Variant(const Vector<int>& p_array) {

	type=NIL;
	DVector<int> v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

Variant::Variant(const Vector<real_t>& p_array) {

	type=NIL;
	DVector<real_t> v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

Variant::Variant(const Vector<String>& p_array) {

	type=NIL;
	DVector<String> v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

Variant::Variant(const Vector<Vector3>& p_array) {

	type=NIL;
	DVector<Vector3> v;
	int len=p_array.size();
	if (len>0) {
		v.resize(len);
		DVector<Vector3>::Write w = v.write();
		const Vector3 *r = p_array.ptr();

		for (int i=0;i<len;i++)
			w[i]=r[i];
	}
	*this=v;
}

Variant::Variant(const Vector<Color>& p_array) {

	type=NIL;
	DVector<Color> v;
	int len=p_array.size();
	v.resize(len);
	for (int i=0;i<len;i++)
		v.set(i,p_array[i]);
	*this=v;
}

void Variant::operator=(const Variant& p_variant) {

	reference(p_variant);
}

Variant::Variant(const IP_Address& p_address) {

	type=STRING;
	memnew_placement( _data._mem, String( p_address ) );
}

Variant::Variant(const Variant& p_variant) {

	type=NIL;
	reference(p_variant);
}


/*
Variant::~Variant() {

	clear();
}*/

uint32_t Variant::hash() const {

	switch( type ) {
		case NIL: {

			return 0;
		} break;
		case BOOL: {

			return _data._bool?1:0;
		} break;
		case INT: {

			return _data._int;

		} break;
		case REAL: {

			MarshallFloat mf;
			mf.f=_data._real;
			return mf.i;

		} break;
		case STRING: {

			return reinterpret_cast<const String*>(_data._mem)->hash();
		} break;
			// math types

		case VECTOR2: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Vector2*>(_data._mem)->x);
			return hash_djb2_one_float(reinterpret_cast<const Vector2*>(_data._mem)->y,hash);
		} break;
		case RECT2: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Rect2*>(_data._mem)->pos.x);
			hash = hash_djb2_one_float(reinterpret_cast<const Rect2*>(_data._mem)->pos.y,hash);
			hash = hash_djb2_one_float(reinterpret_cast<const Rect2*>(_data._mem)->size.x,hash);
			return hash_djb2_one_float(reinterpret_cast<const Rect2*>(_data._mem)->size.y,hash);
		} break;
		case MATRIX32: {

			uint32_t hash = 5831;
			for(int i=0;i<3;i++) {

				for(int j=0;j<2;j++) {
					hash = hash_djb2_one_float(_data._matrix32->elements[i][j],hash);
				}
			}

			return hash;
		} break;
		case VECTOR3: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Vector3*>(_data._mem)->x);
			hash = hash_djb2_one_float(reinterpret_cast<const Vector3*>(_data._mem)->y,hash);
			return hash_djb2_one_float(reinterpret_cast<const Vector3*>(_data._mem)->z,hash);
		} break;
		case PLANE: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Plane*>(_data._mem)->normal.x);
			hash = hash_djb2_one_float(reinterpret_cast<const Plane*>(_data._mem)->normal.y,hash);
			hash = hash_djb2_one_float(reinterpret_cast<const Plane*>(_data._mem)->normal.z,hash);
			return hash_djb2_one_float(reinterpret_cast<const Plane*>(_data._mem)->d,hash);

		} break;
	/*
			case QUAT: {


			} break;*/
		case _AABB: {

			uint32_t hash = 5831;
			for(int i=0;i<3;i++) {

				hash = hash_djb2_one_float(_data._aabb->pos[i],hash);
				hash = hash_djb2_one_float(_data._aabb->size[i],hash);
			}


			return hash;

		} break;
		case QUAT: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Quat*>(_data._mem)->x);
			hash = hash_djb2_one_float(reinterpret_cast<const Quat*>(_data._mem)->y,hash);
			hash = hash_djb2_one_float(reinterpret_cast<const Quat*>(_data._mem)->z,hash);
			return hash_djb2_one_float(reinterpret_cast<const Quat*>(_data._mem)->w,hash);

		} break;
		case MATRIX3: {

			uint32_t hash = 5831;
			for(int i=0;i<3;i++) {

				for(int j=0;j<3;j++) {
					hash = hash_djb2_one_float(_data._matrix3->elements[i][j],hash);
				}
			}

			return hash;

		} break;
		case TRANSFORM: {

			uint32_t hash = 5831;
			for(int i=0;i<3;i++) {

				for(int j=0;j<3;j++) {
					hash = hash_djb2_one_float(_data._transform->basis.elements[i][j],hash);
				}
				hash = hash_djb2_one_float(_data._transform->origin[i],hash);
			}


			return hash;

		} break;

			// misc types
		case COLOR: {

			uint32_t hash = hash_djb2_one_float(reinterpret_cast<const Color*>(_data._mem)->r);
			hash = hash_djb2_one_float(reinterpret_cast<const Color*>(_data._mem)->g,hash);
			hash = hash_djb2_one_float(reinterpret_cast<const Color*>(_data._mem)->b,hash);
			return hash_djb2_one_float(reinterpret_cast<const Color*>(_data._mem)->a,hash);

		} break;
		case IMAGE: {

			return 0;

		} break;
		case _RID: {

			return hash_djb2_one_64(reinterpret_cast<const RID*>(_data._mem)->get_id());
		} break;
		case OBJECT: {

			return hash_djb2_one_64(make_uint64_t(_get_obj().obj));
		} break;
		case NODE_PATH: {

			return reinterpret_cast<const NodePath*>(_data._mem)->hash();
		} break;
		case INPUT_EVENT: {

			return hash_djb2_buffer((uint8_t*)_data._input_event,sizeof(InputEvent));

		} break;
		case DICTIONARY: {

				return reinterpret_cast<const Dictionary*>(_data._mem)->hash();


		} break;
		case ARRAY: {

			const Array& arr = *reinterpret_cast<const Array* >(_data._mem);
			return arr.hash();

		} break;
		case RAW_ARRAY: {

			const DVector<uint8_t>& arr = *reinterpret_cast<const DVector<uint8_t>* >(_data._mem);
			int len = arr.size();
			DVector<uint8_t>::Read r = arr.read();

			return hash_djb2_buffer((uint8_t*)&r[0],len);

		} break;
		case INT_ARRAY: {

			const DVector<int>& arr = *reinterpret_cast<const DVector<int>* >(_data._mem);
			int len = arr.size();
			DVector<int>::Read r = arr.read();

			return hash_djb2_buffer((uint8_t*)&r[0],len*sizeof(int));

		} break;
		case REAL_ARRAY: {

			const DVector<real_t>& arr = *reinterpret_cast<const DVector<real_t>* >(_data._mem);
			int len = arr.size();
			DVector<real_t>::Read r = arr.read();

			return hash_djb2_buffer((uint8_t*)&r[0],len*sizeof(real_t));

		} break;
		case STRING_ARRAY: {

			uint32_t hash=5831;
			const DVector<String>& arr = *reinterpret_cast<const DVector<String>* >(_data._mem);
			int len = arr.size();
			DVector<String>::Read r = arr.read();

			for(int i=0;i<len;i++) {
				hash = hash_djb2_one_32(r[i].hash(),hash);
			}

			return hash;
		} break;
		case VECTOR2_ARRAY: {

			uint32_t hash=5831;
			const DVector<Vector2>& arr = *reinterpret_cast<const DVector<Vector2>* >(_data._mem);
			int len = arr.size();
			DVector<Vector2>::Read r = arr.read();

			for(int i=0;i<len;i++) {
				hash = hash_djb2_one_float(r[i].x,hash);
				hash = hash_djb2_one_float(r[i].y,hash);
			}

			return hash;

		} break;
		case VECTOR3_ARRAY: {

			uint32_t hash=5831;
			const DVector<Vector3>& arr = *reinterpret_cast<const DVector<Vector3>* >(_data._mem);
			int len = arr.size();
			DVector<Vector3>::Read r = arr.read();

			for(int i=0;i<len;i++) {
				hash = hash_djb2_one_float(r[i].x,hash);
				hash = hash_djb2_one_float(r[i].y,hash);
				hash = hash_djb2_one_float(r[i].z,hash);
			}

			return hash;

		} break;
		case COLOR_ARRAY: {

			uint32_t hash=5831;
			const DVector<Color>& arr = *reinterpret_cast<const DVector<Color>* >(_data._mem);
			int len = arr.size();
			DVector<Color>::Read r = arr.read();

			for(int i=0;i<len;i++) {
				hash = hash_djb2_one_float(r[i].r,hash);
				hash = hash_djb2_one_float(r[i].g,hash);
				hash = hash_djb2_one_float(r[i].b,hash);
				hash = hash_djb2_one_float(r[i].a,hash);
			}

			return hash;

		} break;
		default: {}

		}

	return 0;

}


bool Variant::is_ref() const {

	return type==OBJECT && !_get_obj().ref.is_null();
}


Vector<Variant> varray() {

	return Vector<Variant>();
}

Vector<Variant> varray(const Variant& p_arg1) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	return v;
}
Vector<Variant> varray(const Variant& p_arg1,const Variant& p_arg2) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	v.push_back(p_arg2);
	return v;
}
Vector<Variant> varray(const Variant& p_arg1,const Variant& p_arg2,const Variant& p_arg3) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	v.push_back(p_arg2);
	v.push_back(p_arg3);
	return v;
}
Vector<Variant> varray(const Variant& p_arg1,const Variant& p_arg2,const Variant& p_arg3,const Variant& p_arg4) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	v.push_back(p_arg2);
	v.push_back(p_arg3);
	v.push_back(p_arg4);
	return v;
}

Vector<Variant> varray(const Variant& p_arg1,const Variant& p_arg2,const Variant& p_arg3,const Variant& p_arg4,const Variant& p_arg5) {

	Vector<Variant> v;
	v.push_back(p_arg1);
	v.push_back(p_arg2);
	v.push_back(p_arg3);
	v.push_back(p_arg4);
	v.push_back(p_arg5);
	return v;

}

void Variant::static_assign(const Variant& p_variant) {


}

bool Variant::is_shared() const {

    switch(type) {

        case OBJECT: return true;
	case ARRAY:	return reinterpret_cast<const Array*>(_data._mem)->is_shared();
        case DICTIONARY: return reinterpret_cast<const Dictionary*>(_data._mem)->is_shared();
        default: {}

    }

    return false;
}

Variant Variant::call(const StringName& p_method,VARIANT_ARG_DECLARE) {
	VARIANT_ARGPTRS;
	int argc=0;
	for(int i=0;i<VARIANT_ARG_MAX;i++) {
		if (argptr[i]->get_type()==Variant::NIL)
			break;
		argc++;
	}

	CallError error;

	Variant ret = call(p_method,argptr,argc,error);

	switch(error.error) {

		case CallError::CALL_ERROR_INVALID_ARGUMENT: {

			String err = "Invalid type for argument #"+itos(error.argument)+", expected '"+Variant::get_type_name(error.expected)+"'.";
			ERR_PRINT(err.utf8().get_data());

		} break;
		case CallError::CALL_ERROR_INVALID_METHOD: {

			String err = "Invalid method '"+p_method+"' for type '"+Variant::get_type_name(type)+"'.";
			ERR_PRINT(err.utf8().get_data());
		} break;
		case CallError::CALL_ERROR_TOO_MANY_ARGUMENTS: {

			String err = "Too many arguments for method '"+p_method+"'";
			ERR_PRINT(err.utf8().get_data());
		} break;
		default: {}
	}

	return ret;
}

