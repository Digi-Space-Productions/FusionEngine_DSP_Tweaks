import os
import sys
import string

def is_active():
	return True
	
def get_name():
	return "JavaScript"

def can_build():
	return True

def get_opts():

	return [
		['compress','Compress JS Executable','no']
	]

def get_flags():

	return [
		('lua', 'no'),
		('tools', 'no'),
		('nedmalloc', 'no'),
		('theora', 'yes'),
		('tools', 'no'),
		('nedmalloc', 'no'),
		('vorbis', 'yes'),
		('musepack', 'no'),
		('squirrel', 'no'),
		('squish', 'no'),
		('speex', 'no'),
		('old_scenes', 'no'),
#		('default_gui_theme', 'no'),

		#('builtin_zlib', 'no'),
	]



def configure(env):


	env.Append(CPPPATH=['#platform/javascript'])
	
	#em_path=os.environ["EMSCRIPTEN_ROOT"]
	
	#env['ENV']['PATH'] = em_path+":"+env['ENV']['PATH']

	env['CC'] = 'emcc'
	env['CXX'] = 'emcc'
	env['AR'] = "emar"
	env['RANLIB'] = "emranlib"

#	env.Append(LIBS=['c','m','stdc++','log','GLESv1_CM','GLESv2'])

#	env["LINKFLAGS"]= string.split(" -g --sysroot="+ld_sysroot+" -Wl,--no-undefined -Wl,-z,noexecstack ")

	if (env["target"]=="release"):

		env.Append(CCFLAGS=['-O2'])

	elif (env["target"]=="release_debug"):

		env.Append(CCFLAGS=['-O2','-DDEBUG_ENABLED'])

	elif (env["target"]=="debug"):
		env.Append(CCFLAGS=['-D_DEBUG', '-Wall', '-O2', '-DDEBUG_ENABLED'])
		env.Append(CPPFLAGS=['-DDEBUG_MEMORY_ALLOC'])

	env.Append(CPPFLAGS=['-DNO_SAFE_CAST','-fno-rtti'])
	env.Append(CPPFLAGS=['-DJAVASCRIPT_ENABLED', '-DUNIX_ENABLED', '-DNO_FCNTL','-DMPC_FIXED_POINT','-DTYPED_METHOD_BIND','-DNO_THREADS'])
	env.Append(CPPFLAGS=['-DGLES2_ENABLED'])
	env.Append(CPPFLAGS=['-DGLES_NO_CLIENT_ARRAYS'])
	env.Append(CPPFLAGS=['-s','FORCE_FILESYSTEM=1'])
	env.Append(CPPFLAGS=['-s','FULL_ES2=1'])
	env.Append(CXXFLAGS=['-std=gnu++03'])
#	env.Append(CPPFLAGS=['-DANDROID_ENABLED', '-DUNIX_ENABLED','-DMPC_FIXED_POINT'])
	if (env["compress"]=="yes"):
		lzma_binpath = em_path+"/third_party/lzma.js/lzma-native"
		lzma_decoder = em_path+"/third_party/lzma.js/lzma-decoder.js"
		lzma_dec = "LZMA.decompress"

		env.Append(LINKFLAGS=['--compression',lzma_binpath+","+lzma_decoder+","+lzma_dec])

	env.Append(LINKFLAGS=['-s','ASM_JS=1'])
	env.Append(LINKFLAGS=['-O2'])


