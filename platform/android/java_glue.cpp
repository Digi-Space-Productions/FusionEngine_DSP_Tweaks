/*************************************************************************/
/*  java_glue.cpp                                                        */
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
#ifndef ANDROID_NATIVE_ACTIVITY

#include "java_glue.h"
#include "os_android.h"
#include "main/main.h"
#include <unistd.h>
#include "file_access_jandroid.h"
#include "dir_access_jandroid.h"
#include "audio_driver_jandroid.h"
#include "globals.h"
#include "thread_jandroid.h"
#include "core/os/keyboard.h"
#include "java_class_wrapper.h"


static JavaClassWrapper *java_class_wrapper=NULL;
static OS_Android *os_android=NULL;


jvalue _variant_to_jvalue(JNIEnv *env, Variant::Type p_type, const Variant* p_arg, bool force_jobject = false) {

	jvalue v;

	switch(p_type) {

		case Variant::BOOL: {

			if (force_jobject) {
				jclass bclass = env->FindClass("java/lang/Boolean");
				jmethodID ctor = env->GetMethodID(bclass, "<init>", "(Z)V");
				jvalue val;
				val.z = (bool)(*p_arg);
				jobject obj = env->NewObjectA(bclass, ctor, &val);
				v.l = obj;
			} else {
				v.z=*p_arg;
			};
		} break;
		case Variant::INT: {

			if (force_jobject) {

				jclass bclass = env->FindClass("java/lang/Integer");
				jmethodID ctor = env->GetMethodID(bclass, "<init>", "(I)V");
				jvalue val;
				val.i = (int)(*p_arg);
				jobject obj = env->NewObjectA(bclass, ctor, &val);
				v.l = obj;

			} else {
				v.i=*p_arg;
			};
		} break;
		case Variant::REAL: {

			if (force_jobject) {

				jclass bclass = env->FindClass("java/lang/Double");
				jmethodID ctor = env->GetMethodID(bclass, "<init>", "(D)V");
				jvalue val;
				val.d = (double)(*p_arg);
				jobject obj = env->NewObjectA(bclass, ctor, &val);
				v.l = obj;

			} else {
				v.f=*p_arg;
			};
		} break;
		case Variant::STRING: {

			String s = *p_arg;
			jstring jStr = env->NewStringUTF(s.utf8().get_data());
			v.l=jStr;
		} break;
		case Variant::STRING_ARRAY: {

			DVector<String> sarray = *p_arg;
			jobjectArray arr = env->NewObjectArray(sarray.size(),env->FindClass("java/lang/String"),env->NewStringUTF(""));

			for(int j=0;j<sarray.size();j++) {

				env->SetObjectArrayElement(arr,j,env->NewStringUTF( sarray[j].utf8().get_data() ));
			}
			v.l=arr;

		} break;

		case Variant::DICTIONARY: {

			Dictionary dict = *p_arg;
			jclass dclass = env->FindClass("com/android/godot/Dictionary");
			jmethodID ctor = env->GetMethodID(dclass, "<init>", "()V");
			jobject jdict = env->NewObject(dclass, ctor);

			Array keys = dict.keys();

			jobjectArray jkeys = env->NewObjectArray(keys.size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));
			for (int j=0; j<keys.size(); j++) {
				env->SetObjectArrayElement(jkeys, j, env->NewStringUTF(String(keys[j]).utf8().get_data()));
			};

			jmethodID set_keys = env->GetMethodID(dclass, "set_keys", "([Ljava/lang/String;)V");
			jvalue val;
			val.l = jkeys;
			env->CallVoidMethodA(jdict, set_keys, &val);

			jobjectArray jvalues = env->NewObjectArray(keys.size(), env->FindClass("java/lang/Object"), NULL);

			for (int j=0; j<keys.size(); j++) {
				Variant var = dict[keys[j]];
				val = _variant_to_jvalue(env, var.get_type(), &var, true);
				env->SetObjectArrayElement(jvalues, j, val.l);
			};

			jmethodID set_values = env->GetMethodID(dclass, "set_values", "([Ljava/lang/Object;)V");
			val.l = jvalues;
			env->CallVoidMethodA(jdict, set_values, &val);

			v.l = jdict;
		} break;

		case Variant::INT_ARRAY: {

			DVector<int> array = *p_arg;
			jintArray arr = env->NewIntArray(array.size());
			DVector<int>::Read r = array.read();
			env->SetIntArrayRegion(arr,0,array.size(),r.ptr());
			v.l=arr;

		} break;
		case Variant::RAW_ARRAY: {
			DVector<uint8_t> array = *p_arg;
			jbyteArray arr = env->NewByteArray(array.size());
			DVector<uint8_t>::Read r = array.read();
			env->SetByteArrayRegion(arr,0,array.size(),reinterpret_cast<const signed char*>(r.ptr()));
			v.l=arr;

		} break;
		case Variant::REAL_ARRAY: {

			DVector<float> array = *p_arg;
			jfloatArray arr = env->NewFloatArray(array.size());
			DVector<float>::Read r = array.read();
			env->SetFloatArrayRegion(arr,0,array.size(),r.ptr());
			v.l=arr;

		} break;
		default: {

			v.i = 0;
		} break;

	}
	return v;
};

String _get_class_name(JNIEnv * env, jclass cls, bool* array) {

	jclass cclass = env->FindClass("java/lang/Class");
	jmethodID getName = env->GetMethodID(cclass, "getName", "()Ljava/lang/String;");
	jstring clsName=(jstring) env->CallObjectMethod(cls, getName);

	if (array) {
		jmethodID isArray = env->GetMethodID(cclass, "isArray", "()Z");
		jboolean isarr = env->CallBooleanMethod(cls, isArray);
		(*array) = isarr ? true : false;
	}

	return env->GetStringUTFChars( clsName, NULL );
};


Variant _jobject_to_variant(JNIEnv * env, jobject obj) {

	jclass c = env->GetObjectClass(obj);
	bool array;
	String name = _get_class_name(env, c, &array);
	//print_line("name is " + name + ", array "+Variant(array));

	if (name == "java.lang.String") {

		return String::utf8(env->GetStringUTFChars( (jstring)obj, NULL ));
	};


	if (name == "[Ljava.lang.String;") {

		jobjectArray arr = (jobjectArray)obj;
		int stringCount = env->GetArrayLength(arr);
		//print_line("String array! " + String::num(stringCount));
		DVector<String> sarr;

		for (int i=0; i<stringCount; i++) {
			jstring string = (jstring) env->GetObjectArrayElement(arr, i);
			const char *rawString = env->GetStringUTFChars(string, 0);
			sarr.push_back(String(rawString));
		}

		return sarr;
	};

	if (name == "java.lang.Boolean") {

		jmethodID boolValue = env->GetMethodID(c, "booleanValue", "()Z");
		bool ret = env->CallBooleanMethod(obj, boolValue);
		return ret;
	};

	if (name == "java.lang.Integer") {

		jclass nclass = env->FindClass("java/lang/Number");
		jmethodID intValue = env->GetMethodID(nclass, "intValue", "()I");
		int ret = env->CallIntMethod(obj, intValue);
		return ret;
	};

	if (name == "[I") {

		jintArray arr = (jintArray)obj;
		int fCount = env->GetArrayLength(arr);
		DVector<int> sarr;
		sarr.resize(fCount);

		DVector<int>::Write w = sarr.write();
		env->GetIntArrayRegion(arr,0,fCount,w.ptr());
		w = DVector<int>::Write();
		return sarr;
	};

	if (name == "[B") {

		jbyteArray arr = (jbyteArray)obj;
		int fCount = env->GetArrayLength(arr);
		DVector<uint8_t> sarr;
		sarr.resize(fCount);

		DVector<uint8_t>::Write w = sarr.write();
		env->GetByteArrayRegion(arr,0,fCount,reinterpret_cast<signed char*>(w.ptr()));
		w = DVector<uint8_t>::Write();
		return sarr;
	};

	if (name == "java.lang.Float" || name == "java.lang.Double") {

		jclass nclass = env->FindClass("java/lang/Number");
		jmethodID doubleValue = env->GetMethodID(nclass, "doubleValue", "()D");
		double ret = env->CallDoubleMethod(obj, doubleValue);
		return ret;
	};

	if (name == "[D") {

		jdoubleArray arr = (jdoubleArray)obj;
		int fCount = env->GetArrayLength(arr);
		PackedFloatArray sarr;
		sarr.resize(fCount);

		PackedFloatArray::Write w = sarr.write();

		for (int i=0; i<fCount; i++) {

			double n;
			env->GetDoubleArrayRegion(arr, i, 1, &n);
			w.ptr()[i] = n;

		};
		return sarr;
	};

	if (name == "[F") {

		jfloatArray arr = (jfloatArray)obj;
		int fCount = env->GetArrayLength(arr);
		PackedFloatArray sarr;
		sarr.resize(fCount);


		PackedFloatArray::Write w = sarr.write();

		for (int i=0; i<fCount; i++) {

			float n;
			env->GetFloatArrayRegion(arr, i, 1, &n);
			w.ptr()[i] = n;

		};
		return sarr;
	};


	if (name == "[Ljava.lang.Object;") {

		jobjectArray arr = (jobjectArray)obj;
		int objCount = env->GetArrayLength(arr);
		Array varr;

		for (int i=0; i<objCount; i++) {
			jobject jobj = env->GetObjectArrayElement(arr, i);
			Variant v = _jobject_to_variant(env, jobj);
			varr.push_back(v);
		}

		return varr;
	};

	if (name == "com.android.godot.Dictionary") {

		Dictionary ret;
		jclass oclass = c;
		jmethodID get_keys = env->GetMethodID(oclass, "get_keys", "()[Ljava/lang/String;");
		jobjectArray arr = (jobjectArray)env->CallObjectMethod(obj, get_keys);

		PackedStringArray keys = _jobject_to_variant(env, arr);

		jmethodID get_values = env->GetMethodID(oclass, "get_values", "()[Ljava/lang/Object;");
		arr = (jobjectArray)env->CallObjectMethod(obj, get_values);

		Array vals = _jobject_to_variant(env, arr);

		//print_line("adding " + String::num(keys.size()) + " to Dictionary!");
		for (int i=0; i<keys.size(); i++) {

			ret[keys[i]] = vals[i];
		};

		return ret;
	};

	return Variant();
};

class JNISingleton : public Object {

	OBJ_TYPE( JNISingleton, Object );


	struct MethodData {


		jmethodID method;
		Variant::Type ret_type;
		Vector<Variant::Type> argtypes;
	};

	jobject instance;
	Map<StringName,MethodData> method_map;

public:

	virtual Variant call(const StringName& p_method,const Variant** p_args,int p_argcount,Variant::CallError &r_error) {

		//print_line("attempt to call "+String(p_method));
		ERR_FAIL_COND_V(!instance,Variant());

		r_error.error=Variant::CallError::CALL_OK;

		Map<StringName,MethodData >::Element *E=method_map.find(p_method);
		if (!E) {

			print_line("no exists");
			r_error.error=Variant::CallError::CALL_ERROR_INVALID_METHOD;
			return Variant();
		}


		int ac = E->get().argtypes.size();
		if (ac<p_argcount) {

			print_line("fewargs");
			r_error.error=Variant::CallError::CALL_ERROR_TOO_FEW_ARGUMENTS;
			r_error.argument=ac;
			return Variant();
		}

		if (ac>p_argcount) {

			print_line("manyargs");
			r_error.error=Variant::CallError::CALL_ERROR_TOO_MANY_ARGUMENTS;
			r_error.argument=ac;
			return Variant();
		}



		for(int i=0;i<p_argcount;i++) {

			if (!Variant::can_convert(p_args[i]->get_type(),E->get().argtypes[i])) {

				r_error.error=Variant::CallError::CALL_ERROR_INVALID_ARGUMENT;
				r_error.argument=i;
				r_error.expected=E->get().argtypes[i];
			}
		}


		jvalue *v=NULL;

		if (p_argcount) {

			v=(jvalue*)alloca( sizeof(jvalue)*p_argcount );
		}

		JNIEnv *env = ThreadAndroid::get_env();

		//print_line("argcount "+String::num(p_argcount));
		for(int i=0;i<p_argcount;i++) {

			v[i] = _variant_to_jvalue(env, E->get().argtypes[i], p_args[i]);
		}

		//print_line("calling method!!");

		Variant ret;

		switch(E->get().ret_type) {

			case Variant::NIL: {


				//print_line("call void");
				env->CallVoidMethodA(instance,E->get().method,v);
			} break;
			case Variant::BOOL: {

				ret = env->CallBooleanMethodA(instance,E->get().method,v);
				//print_line("call bool");
			} break;
			case Variant::INT: {

				ret = env->CallIntMethodA(instance,E->get().method,v);
				//print_line("call int");
			} break;
			case Variant::REAL: {

				ret = env->CallFloatMethodA(instance,E->get().method,v);
			} break;
			case Variant::STRING: {

				jobject o = env->CallObjectMethodA(instance,E->get().method,v);
				String str = env->GetStringUTFChars((jstring)o, NULL );
				ret=str;
			} break;
			case Variant::STRING_ARRAY: {

				jobjectArray arr = (jobjectArray)env->CallObjectMethodA(instance,E->get().method,v);

				ret = _jobject_to_variant(env, arr);

			} break;
			case Variant::INT_ARRAY: {

				jintArray arr = (jintArray)env->CallObjectMethodA(instance,E->get().method,v);

				int fCount = env->GetArrayLength(arr);
				DVector<int> sarr;
				sarr.resize(fCount);

				DVector<int>::Write w = sarr.write();
				env->GetIntArrayRegion(arr,0,fCount,w.ptr());
				w = DVector<int>::Write();
				ret=sarr;
			} break;
			case Variant::REAL_ARRAY: {

				jfloatArray arr = (jfloatArray)env->CallObjectMethodA(instance,E->get().method,v);

				int fCount = env->GetArrayLength(arr);
				DVector<float> sarr;
				sarr.resize(fCount);

				DVector<float>::Write w = sarr.write();
				env->GetFloatArrayRegion(arr,0,fCount,w.ptr());
				w = DVector<float>::Write();
				ret=sarr;
			} break;

			case Variant::DICTIONARY: {

				//print_line("call dictionary");
				jobject obj = env->CallObjectMethodA(instance, E->get().method, v);
				ret = _jobject_to_variant(env, obj);

			} break;
			default: {


				print_line("failure..");
				ERR_FAIL_V(Variant());
			} break;
		}

		//print_line("success");

		return ret;
	}


	jobject get_instance() const {

		return instance;
	}
	void set_instance(jobject p_instance) {

		instance=p_instance;
	}


	void add_method(const StringName& p_name, jmethodID p_method,const Vector<Variant::Type>& p_args, Variant::Type p_ret_type) {

		MethodData md;
		md.method=p_method;
		md.argtypes=p_args;
		md.ret_type=p_ret_type;
		method_map[p_name]=md;

	}


	JNISingleton() {
		instance=NULL;

	}

};


struct TST {

	int a;
	TST() {

		a=5;
	}
};

TST tst;

struct JAndroidPointerEvent {

	Vector<OS_Android::TouchPos> points;
	int pointer;
	int what;
};

static List<JAndroidPointerEvent> pointer_events;
static List<InputEvent> key_events;
static bool initialized=false;
static Mutex *input_mutex=NULL;
static Mutex *suspend_mutex=NULL;
static int step=0;
static bool resized=false;
static bool resized_reload=false;
static bool quit_request=false;
static Size2 new_size;
static Vector3 accelerometer;
static HashMap<String,JNISingleton*> jni_singletons;
static jobject godot_io;

static Vector<int> joy_device_ids;

typedef void (*GFXInitFunc)(void *ud,bool gl2);

static jmethodID _on_video_init=0;
static jobject _godot_instance;

static jmethodID _openURI=0;
static jmethodID _getDataDir=0;
static jmethodID _getLocale=0;
static jmethodID _getModel=0;
static jmethodID _showKeyboard=0;
static jmethodID _hideKeyboard=0;
static jmethodID _setScreenOrientation=0;
static jmethodID _getUniqueID=0;
static jmethodID _getSystemDir=0;
static jmethodID _playVideo=0;
static jmethodID _isVideoPlaying=0;
static jmethodID _pauseVideo=0;
static jmethodID _stopVideo=0;


static void _gfx_init_func(void* ud, bool gl2) {

}


static int _open_uri(const String& p_uri) {

	JNIEnv *env = ThreadAndroid::get_env();
	jstring jStr = env->NewStringUTF(p_uri.utf8().get_data());
	return env->CallIntMethod(godot_io,_openURI,jStr)	;
}

static String _get_data_dir() {

	JNIEnv *env = ThreadAndroid::get_env();
	jstring s =(jstring)env->CallObjectMethod(godot_io,_getDataDir);
	return String(env->GetStringUTFChars( s, NULL ));
}

static String _get_locale() {

	JNIEnv *env = ThreadAndroid::get_env();
	jstring s =(jstring)env->CallObjectMethod(godot_io,_getLocale);
	return String(env->GetStringUTFChars( s, NULL ));
}

static String _get_model() {

    JNIEnv *env = ThreadAndroid::get_env();
    jstring s =(jstring)env->CallObjectMethod(godot_io,_getModel);
    return String(env->GetStringUTFChars( s, NULL ));
}

static String _get_unique_id() {

    JNIEnv *env = ThreadAndroid::get_env();
    jstring s =(jstring)env->CallObjectMethod(godot_io,_getUniqueID);
    return String(env->GetStringUTFChars( s, NULL ));
}

static void _show_vk(const String& p_existing) {

	JNIEnv* env = ThreadAndroid::get_env();
	jstring jStr = env->NewStringUTF(p_existing.utf8().get_data());
	env->CallVoidMethod(godot_io, _showKeyboard, jStr);
};

static void _set_screen_orient(int p_orient) {

	JNIEnv* env = ThreadAndroid::get_env();
	env->CallVoidMethod(godot_io, _setScreenOrientation, p_orient );
};

static String _get_system_dir(int p_dir) {

	JNIEnv *env = ThreadAndroid::get_env();
	jstring s =(jstring)env->CallObjectMethod(godot_io,_getSystemDir,p_dir);
	return String(env->GetStringUTFChars( s, NULL ));
};


static void _hide_vk() {

	JNIEnv* env = ThreadAndroid::get_env();
	env->CallVoidMethod(godot_io, _hideKeyboard);
};

// virtual Error native_video_play(String p_path);
// virtual bool native_video_is_playing();
// virtual void native_video_pause();
// virtual void native_video_stop();

static void _play_video(const String& p_path) {

}

static bool _is_video_playing() {
	JNIEnv* env = ThreadAndroid::get_env();
	return env->CallBooleanMethod(godot_io, _isVideoPlaying);
	//return false;
}

static void _pause_video() {
	JNIEnv* env = ThreadAndroid::get_env();
	env->CallVoidMethod(godot_io, _pauseVideo);
}

static void _stop_video() {
	JNIEnv* env = ThreadAndroid::get_env();
	env->CallVoidMethod(godot_io, _stopVideo);
}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_initialize(JNIEnv * env, jobject obj, jobject activity,jboolean p_need_reload_hook, jobjectArray p_cmdline) {

	__android_log_print(ANDROID_LOG_INFO,"godot","**INIT EVENT! - %p\n",env);


	initialized=true;

	JavaVM *jvm;
	env->GetJavaVM(&jvm);

	_godot_instance=env->NewGlobalRef(activity);
//	_godot_instance=activity;

	__android_log_print(ANDROID_LOG_INFO,"godot","***************** HELLO FROM JNI!!!!!!!!");

	{
		//setup IO Object

		jclass cls = env->FindClass("com/android/godot/Godot");
		if (cls) {

			cls=(jclass)env->NewGlobalRef(cls);
			__android_log_print(ANDROID_LOG_INFO,"godot","*******CLASS FOUND!!!");
		}

		__android_log_print(ANDROID_LOG_INFO,"godot","STEP2, %p",cls);
		jfieldID fid = env->GetStaticFieldID(cls, "io", "Lcom/android/godot/GodotIO;");
		__android_log_print(ANDROID_LOG_INFO,"godot","STEP3 %i",fid);
		jobject ob = env->GetStaticObjectField(cls,fid);
		__android_log_print(ANDROID_LOG_INFO,"godot","STEP4, %p",ob);
		jobject gob = env->NewGlobalRef(ob);

		__android_log_print(ANDROID_LOG_INFO,"godot","STEP4.5, %p",gob);
		godot_io=gob;

		_on_video_init = env->GetMethodID(cls, "onVideoInit", "(Z)V");

		jclass clsio = env->FindClass("com/android/godot/Godot");
		if (cls) {
			jclass c = env->GetObjectClass(gob);
			_openURI = env->GetMethodID(c,"openURI","(Ljava/lang/String;)I");
			_getDataDir = env->GetMethodID(c,"getDataDir","()Ljava/lang/String;");
			_getLocale = env->GetMethodID(c,"getLocale","()Ljava/lang/String;");
			_getModel = env->GetMethodID(c,"getModel","()Ljava/lang/String;");
			_getUniqueID = env->GetMethodID(c,"getUniqueID","()Ljava/lang/String;");
			_showKeyboard = env->GetMethodID(c,"showKeyboard","(Ljava/lang/String;)V");
			_hideKeyboard = env->GetMethodID(c,"hideKeyboard","()V");
			_setScreenOrientation = env->GetMethodID(c,"setScreenOrientation","(I)V");
			_getSystemDir = env->GetMethodID(c,"getSystemDir","(I)Ljava/lang/String;");
			_playVideo = env->GetMethodID(c,"playVideo","(Ljava/lang/String;)V");
			_isVideoPlaying = env->GetMethodID(c,"isVideoPlaying","()Z");
			_pauseVideo = env->GetMethodID(c,"pauseVideo","()V");
			_stopVideo = env->GetMethodID(c,"stopVideo","()V");
		}

		ThreadAndroid::make_default(jvm);
		FileAccessJAndroid::setup(gob);
		DirAccessJAndroid::setup(gob);
		AudioDriverAndroid::setup(gob);
	}


	const char ** cmdline=NULL;
	int cmdlen=0;
	bool use_apk_expansion=false;
	if (p_cmdline) {
		cmdlen = env->GetArrayLength(p_cmdline);
		if (cmdlen) {
			cmdline = (const char**)malloc((env->GetArrayLength(p_cmdline)+1)*sizeof(const char*));
			cmdline[cmdlen]=NULL;

			for (int i=0; i<cmdlen; i++) {

				jstring string = (jstring) env->GetObjectArrayElement(p_cmdline, i);
				const char *rawString = env->GetStringUTFChars(string, 0);
				if (!rawString) {
					__android_log_print(ANDROID_LOG_INFO,"godot","cmdline arg %i is null\n",i);
				} else {
		//			__android_log_print(ANDROID_LOG_INFO,"godot","cmdline arg %i is: %s\n",i,rawString);

					if (strcmp(rawString,"-main_pack")==0)
						use_apk_expansion=true;
				}

				cmdline[i]=rawString;
			}
		}
	}

	__android_log_print(ANDROID_LOG_INFO,"godot","CMDLINE LEN %i - APK EXPANSION %I\n",cmdlen,int(use_apk_expansion));

	os_android = new OS_Android(_gfx_init_func,env,_open_uri,_get_data_dir,_get_locale, _get_model,_show_vk, _hide_vk,_set_screen_orient,_get_unique_id, _get_system_dir, _play_video,_is_video_playing, _pause_video, _stop_video,use_apk_expansion);
	os_android->set_need_reload_hooks(p_need_reload_hook);

	char wd[500];
	getcwd(wd,500);

	__android_log_print(ANDROID_LOG_INFO,"godot","test construction %i\n",tst.a);
	__android_log_print(ANDROID_LOG_INFO,"godot","running from dir %s\n",wd);

	__android_log_print(ANDROID_LOG_INFO,"godot","**SETUP");


#if 0
	char *args[]={"-test","render",NULL};
	__android_log_print(ANDROID_LOG_INFO,"godot","pre asdasd setup...");
	Error err  = Main::setup("apk",2,args,false);
#else
	Error err  = Main::setup("apk",cmdlen,(char**)cmdline,false);
#endif

	if (err!=OK) {
		__android_log_print(ANDROID_LOG_INFO,"godot","*****UNABLE TO SETUP");

		return; //should exit instead and print the error
	}

	__android_log_print(ANDROID_LOG_INFO,"godot","*****SETUP OK");

	//video driver is determined here, because once initialized, it cant be changed
	String vd = Globals::get_singleton()->get("display/driver");


	if (vd.to_upper()=="GLES1")
		env->CallVoidMethod(_godot_instance, _on_video_init, (jboolean)false);
	else
		env->CallVoidMethod(_godot_instance, _on_video_init, (jboolean)true);

	__android_log_print(ANDROID_LOG_INFO,"godot","**START");

	input_mutex=Mutex::create();
	suspend_mutex=Mutex::create();


}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_resize(JNIEnv * env, jobject obj,  jint width, jint height, jboolean reload) {

	__android_log_print(ANDROID_LOG_INFO,"godot","^_^_^_^_^ resize %lld, %i, %i\n",Thread::get_caller_ID(),width,height);
	if (os_android)
		os_android->set_display_size(Size2(width,height));

	/*input_mutex->lock();
	resized=true;
	if (reload)
		resized_reload=true;
	new_size=Size2(width,height);
	input_mutex->unlock();*/

}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_newcontext(JNIEnv * env, jobject obj) {

	__android_log_print(ANDROID_LOG_INFO,"godot","^_^_^_^_^ newcontext %lld\n",Thread::get_caller_ID());
	if (os_android && step > 0) {

		os_android->reload_gfx();
	}

}


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_quit(JNIEnv * env, jobject obj) {

	input_mutex->lock();
	quit_request=true;
	print_line("BACK PRESSED");
	input_mutex->unlock();

}

static void _initialize_java_modules() {


	String modules = Globals::get_singleton()->get("android/modules");
	Vector<String> mods = modules.split(",",false);
	__android_log_print(ANDROID_LOG_INFO,"godot","mod count: %i",mods.size());

	if (mods.size()) {

		JNIEnv *env = ThreadAndroid::get_env();

		jclass activityClass = env->FindClass("com/android/godot/Godot");

		jmethodID getClassLoader = env->GetMethodID(activityClass,"getClassLoader", "()Ljava/lang/ClassLoader;");

		jobject cls = env->CallObjectMethod(_godot_instance, getClassLoader);
		//cls=env->NewGlobalRef(cls);

		jclass classLoader = env->FindClass("java/lang/ClassLoader");
		//classLoader=(jclass)env->NewGlobalRef(classLoader);

		jmethodID findClass = env->GetMethodID(classLoader, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");

		for (int i=0;i<mods.size();i++) {

			String m = mods[i];
			//jclass singletonClass = env->FindClass(m.utf8().get_data());

			print_line("LOADING MODULE: "+m);
			jstring strClassName = env->NewStringUTF(m.utf8().get_data());
			jclass singletonClass = (jclass)env->CallObjectMethod(cls, findClass, strClassName);

			if (!singletonClass) {

				ERR_EXPLAIN("Couldn't find singleton for class: "+m);
				ERR_CONTINUE(!singletonClass);
			}
			//singletonClass=(jclass)env->NewGlobalRef(singletonClass);

			__android_log_print(ANDROID_LOG_INFO,"godot","****^*^*?^*^*class data %x",singletonClass);
			jmethodID initialize = env->GetStaticMethodID(singletonClass, "initialize", "(Landroid/app/Activity;)Lcom/android/godot/Godot$SingletonBase;");

			if (!initialize) {

				ERR_EXPLAIN("Couldn't find proper initialize function 'public static Godot.SingletonBase Class::initialize(Activity p_activity)' initializer for singleton class: "+m);
				ERR_CONTINUE(!initialize);

			}
			jobject obj = env->CallStaticObjectMethod(singletonClass,initialize,_godot_instance);
			__android_log_print(ANDROID_LOG_INFO,"godot","****^*^*?^*^*class instance %x",obj);
			jobject gob = env->NewGlobalRef(obj);


		}

	}

}


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_step(JNIEnv * env, jobject obj)
{


	ThreadAndroid::setup_thread();

	//__android_log_print(ANDROID_LOG_INFO,"godot","**STEP EVENT! - %p-%i\n",env,Thread::get_caller_ID());


	suspend_mutex->lock();
	input_mutex->lock();
	//first time step happens, initialize
	if (step == 0) {
		// ugly hack to initialize the rest of the engine
		// because of the way android forces you to do everything with threads

		java_class_wrapper = memnew( JavaClassWrapper(_godot_instance ));
		Globals::get_singleton()->add_singleton(Globals::Singleton("JavaClassWrapper",java_class_wrapper));
		_initialize_java_modules();

		Main::setup2();
		++step;
		suspend_mutex->unlock();
		input_mutex->unlock();
		return;
	};
	if (step == 1) {
		if (!Main::start()) {

			input_mutex->unlock();
			suspend_mutex->lock();
			return; //should exit instead and print the error
		}

		os_android->main_loop_begin();
		++step;
	}

	while(pointer_events.size()) {

		JAndroidPointerEvent jpe=pointer_events.front()->get();
		os_android->process_touch(jpe.what,jpe.pointer,jpe.points);

		pointer_events.pop_front();
	}

	while (key_events.size()) {

		InputEvent event = key_events.front()->get();
		os_android->process_event(event);

		key_events.pop_front();
	};

	if (quit_request) {

		os_android->main_loop_request_quit();
		quit_request=false;
	}


	input_mutex->unlock();

	os_android->process_accelerometer(accelerometer);

	if (os_android->main_loop_iterate()==true) {

		jclass cls = env->FindClass("com/android/godot/Godot");
		jmethodID _finish = env->GetMethodID(cls, "forceQuit", "()V");
		env->CallVoidMethod(_godot_instance, _finish);
		__android_log_print(ANDROID_LOG_INFO,"godot","**FINISH REQUEST!!! - %p-%i\n",env,Thread::get_caller_ID());

	}

	suspend_mutex->unlock();

}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_touch(JNIEnv * env, jobject obj, jint ev,jint pointer, jint count, jintArray positions) {



	//__android_log_print(ANDROID_LOG_INFO,"godot","**TOUCH EVENT! - %p-%i\n",env,Thread::get_caller_ID());


	Vector<OS_Android::TouchPos> points;
	for(int i=0;i<count;i++) {

		jint p[3];
		env->GetIntArrayRegion(positions,i*3,3,p);
		OS_Android::TouchPos tp;
		tp.pos=Point2(p[1],p[2]);
		tp.id=p[0];
		points.push_back(tp);
	}

	JAndroidPointerEvent jpe;
	jpe.pointer=pointer;
	jpe.points=points;
	jpe.what=ev;

	input_mutex->lock();

	pointer_events.push_back(jpe);

	input_mutex->unlock();
	//if (os_android)
//		os_android->process_touch(ev,pointer,points);

}

/*
 * Android Key codes.
 */
enum {
    AKEYCODE_UNKNOWN         = 0,
    AKEYCODE_SOFT_LEFT       = 1,
    AKEYCODE_SOFT_RIGHT      = 2,
    AKEYCODE_HOME            = 3,
    AKEYCODE_BACK            = 4,
    AKEYCODE_CALL            = 5,
    AKEYCODE_ENDCALL         = 6,
    AKEYCODE_0               = 7,
    AKEYCODE_1               = 8,
    AKEYCODE_2               = 9,
    AKEYCODE_3               = 10,
    AKEYCODE_4               = 11,
    AKEYCODE_5               = 12,
    AKEYCODE_6               = 13,
    AKEYCODE_7               = 14,
    AKEYCODE_8               = 15,
    AKEYCODE_9               = 16,
    AKEYCODE_STAR            = 17,
    AKEYCODE_POUND           = 18,
    AKEYCODE_DPAD_UP         = 19,
    AKEYCODE_DPAD_DOWN       = 20,
    AKEYCODE_DPAD_LEFT       = 21,
    AKEYCODE_DPAD_RIGHT      = 22,
    AKEYCODE_DPAD_CENTER     = 23,
    AKEYCODE_VOLUME_UP       = 24,
    AKEYCODE_VOLUME_DOWN     = 25,
    AKEYCODE_POWER           = 26,
    AKEYCODE_CAMERA          = 27,
    AKEYCODE_CLEAR           = 28,
    AKEYCODE_A               = 29,
    AKEYCODE_B               = 30,
    AKEYCODE_C               = 31,
    AKEYCODE_D               = 32,
    AKEYCODE_E               = 33,
    AKEYCODE_F               = 34,
    AKEYCODE_G               = 35,
    AKEYCODE_H               = 36,
    AKEYCODE_I               = 37,
    AKEYCODE_J               = 38,
    AKEYCODE_K               = 39,
    AKEYCODE_L               = 40,
    AKEYCODE_M               = 41,
    AKEYCODE_N               = 42,
    AKEYCODE_O               = 43,
    AKEYCODE_P               = 44,
    AKEYCODE_Q               = 45,
    AKEYCODE_R               = 46,
    AKEYCODE_S               = 47,
    AKEYCODE_T               = 48,
    AKEYCODE_U               = 49,
    AKEYCODE_V               = 50,
    AKEYCODE_W               = 51,
    AKEYCODE_X               = 52,
    AKEYCODE_Y               = 53,
    AKEYCODE_Z               = 54,
    AKEYCODE_COMMA           = 55,
    AKEYCODE_PERIOD          = 56,
    AKEYCODE_ALT_LEFT        = 57,
    AKEYCODE_ALT_RIGHT       = 58,
    AKEYCODE_SHIFT_LEFT      = 59,
    AKEYCODE_SHIFT_RIGHT     = 60,
    AKEYCODE_TAB             = 61,
    AKEYCODE_SPACE           = 62,
    AKEYCODE_SYM             = 63,
    AKEYCODE_EXPLORER        = 64,
    AKEYCODE_ENVELOPE        = 65,
    AKEYCODE_ENTER           = 66,
    AKEYCODE_DEL             = 67,
    AKEYCODE_GRAVE           = 68,
    AKEYCODE_MINUS           = 69,
    AKEYCODE_EQUALS          = 70,
    AKEYCODE_LEFT_BRACKET    = 71,
    AKEYCODE_RIGHT_BRACKET   = 72,
    AKEYCODE_BACKSLASH       = 73,
    AKEYCODE_SEMICOLON       = 74,
    AKEYCODE_APOSTROPHE      = 75,
    AKEYCODE_SLASH           = 76,
    AKEYCODE_AT              = 77,
    AKEYCODE_NUM             = 78,
    AKEYCODE_HEADSETHOOK     = 79,
    AKEYCODE_FOCUS           = 80,   // *Camera3D* focus
    AKEYCODE_PLUS            = 81,
    AKEYCODE_MENU            = 82,
    AKEYCODE_NOTIFICATION    = 83,
    AKEYCODE_SEARCH          = 84,
    AKEYCODE_MEDIA_PLAY_PAUSE= 85,
    AKEYCODE_MEDIA_STOP      = 86,
    AKEYCODE_MEDIA_NEXT      = 87,
    AKEYCODE_MEDIA_PREVIOUS  = 88,
    AKEYCODE_MEDIA_REWIND    = 89,
    AKEYCODE_MEDIA_FAST_FORWARD = 90,
    AKEYCODE_MUTE            = 91,
    AKEYCODE_PAGE_UP         = 92,
    AKEYCODE_PAGE_DOWN       = 93,
    AKEYCODE_PICTSYMBOLS     = 94,
    AKEYCODE_SWITCH_CHARSET  = 95,
    AKEYCODE_BUTTON_A        = 96,
    AKEYCODE_BUTTON_B        = 97,
    AKEYCODE_BUTTON_C        = 98,
    AKEYCODE_BUTTON_X        = 99,
    AKEYCODE_BUTTON_Y        = 100,
    AKEYCODE_BUTTON_Z        = 101,
    AKEYCODE_BUTTON_L1       = 102,
    AKEYCODE_BUTTON_R1       = 103,
    AKEYCODE_BUTTON_L2       = 104,
    AKEYCODE_BUTTON_R2       = 105,
    AKEYCODE_BUTTON_THUMBL   = 106,
    AKEYCODE_BUTTON_THUMBR   = 107,
    AKEYCODE_BUTTON_START    = 108,
    AKEYCODE_BUTTON_SELECT   = 109,
    AKEYCODE_BUTTON_MODE     = 110,

    // NOTE: If you add a new keycode here you must also add it to several other files.
    //       Refer to frameworks/base/core/java/android/view/KeyEvent.java for the full list.
};

struct _WinTranslatePair {

	unsigned int keysym;
	unsigned int keycode;
};


static _WinTranslatePair _ak_to_keycode[]={
{ KEY_TAB,				AKEYCODE_TAB },
{ KEY_ENTER,			AKEYCODE_ENTER },
{ KEY_SHIFT,			AKEYCODE_SHIFT_LEFT },
{ KEY_SHIFT,			AKEYCODE_SHIFT_RIGHT },
{ KEY_ALT,				AKEYCODE_ALT_LEFT },
{ KEY_ALT,				AKEYCODE_ALT_RIGHT },
{ KEY_MENU,				AKEYCODE_MENU },
{ KEY_PAUSE,			AKEYCODE_MEDIA_PLAY_PAUSE },
{ KEY_ESCAPE,			AKEYCODE_BACK },
{ KEY_SPACE,			AKEYCODE_SPACE },
{ KEY_PAGEUP,			AKEYCODE_PAGE_UP },
{ KEY_PAGEDOWN,			AKEYCODE_PAGE_DOWN },
{ KEY_HOME,				AKEYCODE_HOME },//(0x24)
{ KEY_LEFT,				AKEYCODE_DPAD_LEFT },
{ KEY_UP,				AKEYCODE_DPAD_UP },
{ KEY_RIGHT,			AKEYCODE_DPAD_RIGHT },
{ KEY_DOWN,				AKEYCODE_DPAD_DOWN},
{ KEY_PERIODCENTERED,	AKEYCODE_DPAD_CENTER },
{ KEY_BACKSPACE,		AKEYCODE_DEL},
{ KEY_0,				AKEYCODE_0 },////0 key
{ KEY_1,				AKEYCODE_1 },////1 key
{ KEY_2,				AKEYCODE_2 },////2 key
{ KEY_3,				AKEYCODE_3 },////3 key
{ KEY_4,				AKEYCODE_4 },////4 key
{ KEY_5,				AKEYCODE_5 },////5 key
{ KEY_6,				AKEYCODE_6 },////6 key
{ KEY_7,				AKEYCODE_7 },////7 key
{ KEY_8,				AKEYCODE_8 },////8 key
{ KEY_9,				AKEYCODE_9 },////9 key
{ KEY_A,				AKEYCODE_A },////A key
{ KEY_B,				AKEYCODE_B },////B key
{ KEY_C,				AKEYCODE_C },////C key
{ KEY_D,				AKEYCODE_D },////D key
{ KEY_E,				AKEYCODE_E },////E key
{ KEY_F,				AKEYCODE_F },////F key
{ KEY_G,				AKEYCODE_G },////G key
{ KEY_H,				AKEYCODE_H },////H key
{ KEY_I,				AKEYCODE_I },////I key
{ KEY_J,				AKEYCODE_J },////J key
{ KEY_K,				AKEYCODE_K },////K key
{ KEY_L,				AKEYCODE_L },////L key
{ KEY_M,				AKEYCODE_M },////M key
{ KEY_N,				AKEYCODE_N },////N key
{ KEY_O,				AKEYCODE_O },////O key
{ KEY_P,				AKEYCODE_P },////P key
{ KEY_Q,				AKEYCODE_Q },////Q key
{ KEY_R,				AKEYCODE_R },////R key
{ KEY_S,				AKEYCODE_S },////S key
{ KEY_T,				AKEYCODE_T },////T key
{ KEY_U,				AKEYCODE_U },////U key
{ KEY_V,				AKEYCODE_V },////V key
{ KEY_W,				AKEYCODE_W },////W key
{ KEY_X,				AKEYCODE_X },////X key
{ KEY_Y,				AKEYCODE_Y },////Y key
{ KEY_Z,				AKEYCODE_Z },////Z key
{ KEY_HOMEPAGE,			AKEYCODE_EXPLORER},
{ KEY_LAUNCH0,			AKEYCODE_BUTTON_A},
{ KEY_LAUNCH1,			AKEYCODE_BUTTON_B},
{ KEY_LAUNCH2,			AKEYCODE_BUTTON_C},
{ KEY_LAUNCH3,			AKEYCODE_BUTTON_X},
{ KEY_LAUNCH4,			AKEYCODE_BUTTON_Y},
{ KEY_LAUNCH5,			AKEYCODE_BUTTON_Z},
{ KEY_LAUNCH6,			AKEYCODE_BUTTON_L1},
{ KEY_LAUNCH7,			AKEYCODE_BUTTON_R1},
{ KEY_LAUNCH8,			AKEYCODE_BUTTON_L2},
{ KEY_LAUNCH9,			AKEYCODE_BUTTON_R2},
{ KEY_LAUNCHA,			AKEYCODE_BUTTON_THUMBL},
{ KEY_LAUNCHB,			AKEYCODE_BUTTON_THUMBR},
{ KEY_LAUNCHC,			AKEYCODE_BUTTON_START},
{ KEY_LAUNCHD,			AKEYCODE_BUTTON_SELECT},
{ KEY_LAUNCHE,			AKEYCODE_BUTTON_MODE},
{ KEY_VOLUMEMUTE,		AKEYCODE_MUTE},
{ KEY_VOLUMEDOWN,		AKEYCODE_VOLUME_DOWN},
{ KEY_VOLUMEUP,			AKEYCODE_VOLUME_UP},
{ KEY_BACK,				AKEYCODE_MEDIA_REWIND },
{ KEY_FORWARD,			AKEYCODE_MEDIA_FAST_FORWARD },
{ KEY_MEDIANEXT,		AKEYCODE_MEDIA_NEXT },
{ KEY_MEDIAPREVIOUS,	AKEYCODE_MEDIA_PREVIOUS },
{ KEY_MEDIASTOP,		AKEYCODE_MEDIA_STOP },
{ KEY_PLUS,				AKEYCODE_PLUS },
{ KEY_EQUAL,			AKEYCODE_EQUALS},// the '+' key
{ KEY_COMMA,			AKEYCODE_COMMA},// the ',' key
{ KEY_MINUS,			AKEYCODE_MINUS},// the '-' key
{ KEY_SLASH,			AKEYCODE_SLASH},// the '/?' key
{ KEY_BACKSLASH,		AKEYCODE_BACKSLASH},
{ KEY_BRACKETLEFT,		AKEYCODE_LEFT_BRACKET},
{ KEY_BRACKETRIGHT,		AKEYCODE_RIGHT_BRACKET},
{ KEY_UNKNOWN,			0} };
/*
TODO: map these android key:
    AKEYCODE_SOFT_LEFT       = 1,
    AKEYCODE_SOFT_RIGHT      = 2,
    AKEYCODE_CALL            = 5,
    AKEYCODE_ENDCALL         = 6,
    AKEYCODE_STAR            = 17,
    AKEYCODE_POUND           = 18,
    AKEYCODE_POWER           = 26,
    AKEYCODE_CAMERA          = 27,
    AKEYCODE_CLEAR           = 28,
    AKEYCODE_SYM             = 63,
    AKEYCODE_ENVELOPE        = 65,
    AKEYCODE_GRAVE           = 68,
    AKEYCODE_SEMICOLON       = 74,
    AKEYCODE_APOSTROPHE      = 75,
    AKEYCODE_AT              = 77,
    AKEYCODE_NUM             = 78,
    AKEYCODE_HEADSETHOOK     = 79,
    AKEYCODE_FOCUS           = 80,   // *Camera3D* focus
    AKEYCODE_NOTIFICATION    = 83,
    AKEYCODE_SEARCH          = 84,
    AKEYCODE_PICTSYMBOLS     = 94,
    AKEYCODE_SWITCH_CHARSET  = 95,
*/

static unsigned int android_get_keysym(unsigned int p_code) {
	for(int i=0;_ak_to_keycode[i].keysym!=KEY_UNKNOWN;i++) {

		if (_ak_to_keycode[i].keycode==p_code) {
			//print_line("outcode: " + _ak_to_keycode[i].keysym);

			return _ak_to_keycode[i].keysym;
		}
	}


	return KEY_UNKNOWN;
}

static int find_device(int p_device) {

	for (int i=0; i<joy_device_ids.size(); i++) {

		if (joy_device_ids[i] == p_device) {
			//print_line("found device at "+String::num(i));
			return i;
		};
	};

	//print_line("adding a device at" + String::num(joy_device_ids.size()));
	joy_device_ids.push_back(p_device);

	return joy_device_ids.size() - 1;
};

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_joybutton(JNIEnv * env, jobject obj, jint p_device, jint p_button, jboolean p_pressed) {

	InputEvent ievent;
	ievent.type = InputEvent::JOYSTICK_BUTTON;
	ievent.device = find_device(p_device);
	ievent.joy_button.button_index = p_button;
	ievent.joy_button.pressed = p_pressed;

	input_mutex->lock();
	key_events.push_back(ievent);
	input_mutex->unlock();
};

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_joyaxis(JNIEnv * env, jobject obj, jint p_device, jint p_axis, jfloat p_value) {

	InputEvent ievent;
	ievent.type = InputEvent::JOYSTICK_MOTION;
	ievent.device = find_device(p_device);
	ievent.joy_motion.axis = p_axis;
	ievent.joy_motion.axis_value = p_value;

	input_mutex->lock();
	key_events.push_back(ievent);
	input_mutex->unlock();
};


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_key(JNIEnv * env, jobject obj, jint p_scancode, jint p_unicode_char, jboolean p_pressed) {

	InputEvent ievent;
	ievent.type = InputEvent::KEY;
	ievent.device = 0;
	int val = p_unicode_char;
	int scancode = android_get_keysym(p_scancode);
	ievent.key.scancode = scancode;
	ievent.key.unicode = val;
	ievent.key.pressed = p_pressed;

	print_line("Scancode: " + String::num(p_scancode) + ":" + String::num(ievent.key.scancode) + " Unicode: " + String::num(val));

	ievent.key.mod.shift=false;
	ievent.key.mod.alt=false;
	ievent.key.mod.control=false;
	ievent.key.echo=false;

    if (val == '\n')
    {
		ievent.key.scancode = KEY_ENTER;
    }else if (val == 61448) {
		ievent.key.scancode = KEY_BACKSPACE;
		ievent.key.unicode = KEY_BACKSPACE;
	} else if (val == 61453) {
		ievent.key.scancode = KEY_ENTER;
		ievent.key.unicode = KEY_ENTER;
	} else if (p_scancode==4) {

	    quit_request=true;
    }

	input_mutex->lock();
	key_events.push_back(ievent);
	input_mutex->unlock();
};


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_accelerometer(JNIEnv * env, jobject obj,  jfloat x, jfloat y, jfloat z) {

	input_mutex->lock();
	accelerometer=Vector3(x,y,z);
	input_mutex->unlock();

}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_focusin(JNIEnv * env, jobject obj){

	if (!suspend_mutex)
		return;
	suspend_mutex->lock();

	if (os_android && step > 0)
		os_android->main_loop_focusin();

	suspend_mutex->unlock();
}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_focusout(JNIEnv * env, jobject obj){

	if (!suspend_mutex)
		return;
	suspend_mutex->lock();

	if (os_android && step > 0)
		os_android->main_loop_focusout();

	suspend_mutex->unlock();

}



JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_audio(JNIEnv * env, jobject obj) {

	ThreadAndroid::setup_thread();
	AudioDriverAndroid::thread_func(env);


}


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_singleton(JNIEnv * env, jobject obj, jstring name,jobject p_object){

	String singname = env->GetStringUTFChars( name, NULL );
	JNISingleton *s = memnew( JNISingleton );
	s->set_instance(env->NewGlobalRef(p_object));
	jni_singletons[singname]=s;

	Globals::get_singleton()->add_singleton(Globals::Singleton(singname,s));
	Globals::get_singleton()->set(singname,s);

}


static Variant::Type get_jni_type(const String& p_type) {

	static struct {
		const char *name;
		Variant::Type type;
	} _type_to_vtype[]={
		{"void",Variant::NIL},
		{"boolean",Variant::BOOL},
		{"int",Variant::INT},
		{"float",Variant::REAL},
		{"double", Variant::REAL},
		{"java.lang.String",Variant::STRING},
		{"[I",Variant::INT_ARRAY},
		{"[B",Variant::RAW_ARRAY},
		{"[F",Variant::REAL_ARRAY},
		{"[java.lang.String",Variant::STRING_ARRAY},
		{"com.android.godot.Dictionary", Variant::DICTIONARY},
		{NULL,Variant::NIL}
	};

	int idx=0;

	while (_type_to_vtype[idx].name) {

		if (p_type==_type_to_vtype[idx].name)
			return _type_to_vtype[idx].type;

		idx++;
	}

	return Variant::NIL;
}


static const char* get_jni_sig(const String& p_type) {


	static struct {
		const char *name;
		const char *sig;
	} _type_to_vtype[]={
		{"void","V"},
		{"boolean","Z"},
		{"int","I"},
		{"float","F"},
		{"double","D"},
		{"java.lang.String","Ljava/lang/String;"},
		{"com.android.godot.Dictionary", "Lcom/android/godot/Dictionary;"},
		{"[I","[I"},
		{"[B","[B"},
		{"[F","[F"},
		{"[java.lang.String","[Ljava/lang/String;"},
		{NULL,"V"}
	};

	int idx=0;

	while (_type_to_vtype[idx].name) {

		if (p_type==_type_to_vtype[idx].name)
			return _type_to_vtype[idx].sig;

		idx++;
	}


	return "Ljava/lang/Object;";
}

JNIEXPORT jstring JNICALL Java_com_android_godot_GodotLib_getGlobal(JNIEnv * env, jobject obj, jstring path) {

	String js = env->GetStringUTFChars( path, NULL );

	return env->NewStringUTF(Globals::get_singleton()->get(js).operator String().utf8().get_data());


}


JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_method(JNIEnv * env, jobject obj, jstring sname, jstring name, jstring ret, jobjectArray args){

	String singname = env->GetStringUTFChars( sname, NULL );

	ERR_FAIL_COND(!jni_singletons.has(singname));

	JNISingleton *s = jni_singletons.get(singname);


	String mname = env->GetStringUTFChars( name, NULL );
	String retval = env->GetStringUTFChars( ret, NULL );
	Vector<Variant::Type> types;
	String cs="(";


	int stringCount = env->GetArrayLength(args);

	print_line("Singl:  "+singname+" Method: "+mname+" RetVal: "+retval);
	for (int i=0; i<stringCount; i++) {

		jstring string = (jstring) env->GetObjectArrayElement(args, i);
		const char *rawString = env->GetStringUTFChars(string, 0);
		types.push_back(get_jni_type(String(rawString)));
		cs+=get_jni_sig(String(rawString));
	}

	cs+=")";
	cs+=get_jni_sig(retval);
	jclass cls = env->GetObjectClass(s->get_instance());
	print_line("METHOD: "+mname+" sig: "+cs);
	jmethodID mid = env->GetMethodID(cls, mname.ascii().get_data(), cs.ascii().get_data());
	if (!mid) {

		print_line("FAILED GETTING METHOID "+mname);
	}

	s->add_method(mname,mid,types,get_jni_type(retval));


}

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_callobject(JNIEnv * env, jobject p_obj, jint ID, jstring method, jobjectArray params) {

	String str_method = env->GetStringUTFChars( method, NULL );

	Object* obj = ObjectDB::get_instance(ID);
	ERR_FAIL_COND(!obj);

	int count = env->GetArrayLength(params);
	Variant* vlist = (Variant*)alloca(sizeof(Variant) * count);
	Variant** vptr = (Variant**)alloca(sizeof(Variant*) * count);
	for (int i=0; i<count; i++) {

		jobject obj = env->GetObjectArrayElement(params, i);
		Variant v;
		if (obj)
			v=_jobject_to_variant(env, obj);
		memnew_placement(&vlist[i], Variant);
		vlist[i] = v;
		vptr[i] = &vlist[i];
	};

	Variant::CallError err;
	obj->call(str_method, (const Variant**)vptr, count, err);
	// something
};

JNIEXPORT void JNICALL Java_com_android_godot_GodotLib_calldeferred(JNIEnv * env, jobject p_obj, jint ID, jstring method, jobjectArray params) {

	String str_method = env->GetStringUTFChars( method, NULL );

	Object* obj = ObjectDB::get_instance(ID);
	ERR_FAIL_COND(!obj);

	int count = env->GetArrayLength(params);
	Variant args[VARIANT_ARG_MAX];

//	print_line("Java->GD call: "+obj->get_type()+"::"+str_method+" argc "+itos(count));

	for (int i=0; i<MIN(count,VARIANT_ARG_MAX); i++) {

		jobject obj = env->GetObjectArrayElement(params, i);
		if (obj)
			args[i] = _jobject_to_variant(env, obj);
//		print_line("\targ"+itos(i)+": "+Variant::get_type_name(args[i].get_type()));

	};



	obj->call_deferred(str_method, args[0],args[1],args[2],args[3],args[4]);
	// something
};

//Main::cleanup();

//return os.get_exit_code();
#endif
