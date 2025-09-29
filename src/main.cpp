#include <Python.h>

#include <abstract.h>
#include <cstddef>
#include <cstdlib>
#include <cxxabi.h>
#include <exports.h>
#include <methodobject.h>
#include <modsupport.h>
#include <moduleobject.h>
#include <object.h>
#include <print>
#include <pytypedefs.h>
#include <string>
#include <string_view>
#include <unicodeobject.h>

#define RET_EMPTY_STRING() return PyUnicode_FromString("")

#define RET_NONE() return Py_None

namespace {
	void warn(std::string_view msg) {
		std::println("[demangler] [warn] {}", msg);
	}

	std::string demangle(const std::string& in) {
		auto resLen = 0uz;
		int status = 0;
		auto res =
			__cxxabiv1::__cxa_demangle(in.c_str(), nullptr, &resLen, &status);
		std::string ret = {res, resLen};
		std::free(res);
		return ret;
	}

	namespace demangler {
		PyObject* string(PyObject* self, PyObject* args) {
			auto nArgs = PySequence_Length(args);

			if (nArgs == 0) {
				RET_NONE();
			} else if (nArgs > 1) {
				warn("more than one arg passed to demangle_string");
			}

			auto firstItem = PySequence_GetItem(args, 0);

			if (Py_IsNone(firstItem)) {
				RET_EMPTY_STRING();
			}

			auto inputStrSize = 0Z;
			auto inputStr = PyUnicode_AsUTF8AndSize(firstItem, &inputStrSize);

			if (inputStrSize <= 0 || !inputStr) {
				RET_EMPTY_STRING();
			}

			std::string input = {inputStr, (size_t)inputStrSize};
			auto output = demangle(input);

			return PyUnicode_FromString(output.c_str());
		}

		PyMethodDef methods[] = {
			{.ml_name = "string",
			 .ml_meth = string,
			 .ml_flags = METH_VARARGS,
			 .ml_doc = "Demangle a string. Returns str | None."},
			{.ml_name = nullptr,
			 .ml_meth = nullptr,
			 .ml_flags = 0,
			 .ml_doc = nullptr}};

		PyModuleDef module = {
			.m_base = PyModuleDef_HEAD_INIT,
			.m_name = "demangler",
			.m_doc = "Demangle mangled c++ symbols",
			.m_size = -1,
			.m_methods = methods,
		};
	} // namespace demangler

} // namespace

PyMODINIT_FUNC PyInit_demangler(void) {
	return PyModule_Create(&demangler::module);
}
