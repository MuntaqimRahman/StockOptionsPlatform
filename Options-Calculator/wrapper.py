import ctypes
import glob

lib = glob.glob('build/*/OptionsCalculator*.so')[0]

openlib = ctypes.CDLL(lib)

openlib.calcImpliedVolatility_py.restype = ctypes.c_double
openlib.calcImpliedVolatility_py.argtypes = [ctypes.c_bool, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double, ctypes.c_double]
