from distutils.core import setup, Extension
from pre_install import *

create_py_files()

sketches = Extension('sketches',
                    sources = ['sketchesmodule.cpp'],
                    include_dirs = ['../'],
                    libraries = ['sketches'],
                    library_dirs = ['../'],
                    extra_compile_args = ['-Wno-write-strings', '-std=c++11'])

setup (name = 'sketches',
       version = '1.1',
       description = 'Sketches package',
       author='Ester Lopez',
       url='https://github.com/esterl/sketches/',
       ext_modules = [sketches])
       
