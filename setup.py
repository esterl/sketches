from distutils.core import setup, Extension

sketches = Extension('sketches',
                    sources = ['sketchesmodule.cpp'])

setup (name = 'sketches',
       version = '1.0',
       description = 'Sketches package',
       ext_modules = [sketches])
       
