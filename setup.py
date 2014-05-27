from distutils.core import setup, Extension

sketches = Extension('sketches',
                    sources = ['sketchesmodule.cpp'],
                    extra_compile_args = ['-Wno-write-strings'])

setup (name = 'sketches',
       version = '1.0',
       description = 'Sketches package',
       ext_modules = [sketches])
       
