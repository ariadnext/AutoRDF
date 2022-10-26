import os
import shutil
from pathlib import Path
from setuptools import setup, Extension
from setuptools.command import build_ext


class AutoRdfExtension(Extension):
    def __init__(self, input_file):
        if not os.path.exists(input_file):
            raise ValueError(f'Prebuilt extension file not found: \'{input_file}\'')
        self.input_file = input_file
        super().__init__(Path(input_file).name, ['no-source-needed.c'])


class AutoRdfInstall(build_ext.build_ext):
    def run(self):
        for ext in self.extensions:
            # The build directory must exist
            build_dir = Path(self.build_lib)
            build_dir.mkdir(parents=True, exist_ok=True)

            # We copy the binding library to the build directory
            prebuilt_file = Path(ext.input_file)
            dest_path = build_dir / prebuilt_file.name
            shutil.copyfile(prebuilt_file, dest_path)


ext_module = AutoRdfExtension(os.environ['PREBUILT_FILE'])

setup(
    name='autordf',
    version='1.0.0',
    author='IDnow GmbH',
    author_email='support@idnow.io',
    url='https://github.com/ariadnext/AutoRDF',
    description='AutoRDF Python binding',
    long_description='AutoRDF is open source framework to work with RDF data from a software engineering point of view',
    platforms=[],
    license='LGPL3.0',
    cmdclass={
        'build_ext': AutoRdfInstall,
    },
    ext_modules=[ext_module]
)
