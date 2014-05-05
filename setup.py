from distutils.core import setup, Extension
 
module1 = Extension('soundcoder', sources = ['src/soundcoder.c'], libraries=["mp3lame" , "faac"])
 
setup (name = 'Soundcoder',
	version = '1.0',
        description = 'A fast sound encoder',
        ext_modules = [module1])
