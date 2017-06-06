#include "Parselmouth.h"

#include "utils/SignatureCast.h"

#include <pybind11/stl.h>

#include <experimental/optional>

namespace py = pybind11;
using namespace py::literals;

namespace parselmouth {

enum class Interpolation
{
	NEAREST = Vector_VALUE_INTERPOLATION_NEAREST,
	LINEAR = Vector_VALUE_INTERPOLATION_LINEAR,
	CUBIC = Vector_VALUE_INTERPOLATION_CUBIC,
	SINC70 = Vector_VALUE_INTERPOLATION_SINC70,
	SINC700 = Vector_VALUE_INTERPOLATION_SINC700
};

void Binding<Interpolation>::init() {
	value("NEAREST", Interpolation::NEAREST);
	value("LINEAR", Interpolation::LINEAR);
	value("CUBIC", Interpolation::CUBIC);
	value("SINC70", Interpolation::SINC70);
	value("SINC700", Interpolation::SINC700);

	make_implicitly_convertible_from_string<Interpolation>(*this, true);
}

void Binding<Vector>::init() {
	using signature_cast_placeholder::_;

	// TODO Something to get rid of duplicate functions with different names?
	def("add",
	    &Vector_addScalar,
	    "number"_a);

	def("__iadd__",
	    [](Vector self, double number) { Vector_addScalar(self, number); return self; },
	    "number"_a, py::is_operator());

	def("__add__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
	    "number"_a, py::is_operator());

	def("__radd__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), number); return result; },
	    "number"_a, py::is_operator());

	def("subtract",
	    [](Vector self, double number) { Vector_addScalar(self, -number); },
	    "number"_a);

	def("__isub__",
	    [](Vector self, double number) { Vector_addScalar(self, -number); return self; },
	    "number"_a, py::is_operator());

	def("__sub__",
	    [](Vector self, double number) { auto result = Data_copy(self); Vector_addScalar(result.get(), -number); return result; },
	    "number"_a, py::is_operator());

	def("subtract_mean",
	    &Vector_subtractMean);

	def("multiply",
	    &Vector_multiplyByScalar,
	    "factor"_a);

	def("__imul__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, factor); return self; },
	    "factor"_a, py::is_operator());

	def("__mul__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
	    "factor"_a, py::is_operator());

	def("__rmul__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), factor); return result; },
	    "factor"_a, py::is_operator());

	def("divide", // TODO Not zero?
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); },
	    "factor"_a);

	def("__itruediv__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
	    "factor"_a, py::is_operator());

	def("__truediv__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
	    "factor"_a, py::is_operator());

#if PY_MAJOR_VERSION < 3
	def("__idiv__",
	    [](Vector self, double factor) { Vector_multiplyByScalar(self, 1 / factor); return self; },
	    "factor"_a, py::is_operator());

	def("__div__",
	    [](Vector self, double factor) { auto result = Data_copy(self); Vector_multiplyByScalar(result.get(), 1 / factor); return result; },
	    "factor"_a, py::is_operator());
#endif

	def("scale",
	    signature_cast<_ (_, Positive<double>)>(Vector_scale),
	    "scale"_a);

	def("scale_peak",
	    signature_cast<_ (_, Positive<double>)>(Vector_scale),
	    "new_peak"_a = 0.99);

	def("get_value", // TODO Default for interpolation? Different for Sound (SINC70), Harmonicity/Intensity/Formants (CUBIC) and Ltas (LINEAR); take praat_TimeFunction.h into account
	    [](Vector self, double x, std::experimental::optional<long> channel, Interpolation interpolation) { return Vector_getValueAtX (self, x, channel.value_or(Vector_CHANNEL_AVERAGE), static_cast<int>(interpolation)); },
	    "x"_a, "channel"_a = nullptr, "interpolation"_a = Interpolation::CUBIC);
}

} // namespace parselmouth