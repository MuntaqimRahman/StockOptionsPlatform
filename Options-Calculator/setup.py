from setuptools import setup, Extension

setup(
    ext_modules=[Extension('OptionsCalculator',["Options-Calculator.cpp"],),],
)