#include <sstream>
#include <iomanip>

#include "BindMethods.hpp"

#include <fort/myrmidon/types/Color.hpp>

namespace py = pybind11;

void BindColor(py::module_ &m) {
	using namespace fort::myrmidon;

	m.def(
	    "DefaultPalette",
	    &DefaultPalette,
	    R"pydoc(
Returns **fort-myrmidon** default palette.

In **fort_myrmidon**, a Color is simply a Tuple[:obj:`int`,
:obj:`int`, :obj:`int`].  The default palette has 7 color-blind
friendly colors.

Returns:
    List[Tuple[int,int,int]]: 7 color-blind friendly colors.
)pydoc"
	);

	m.def(
	    "DefaultPaletteColor",
	    &DefaultPaletteColor,
	    py::arg("index"),
	    R"pydoc(
Safely returns one of the :func:`DefaultPalette` color for any index.

Args:
    index(int) : a positive index

Returns:
    Tuple[int,int,int]: a color from :func:`DefaultPalette` wrapping
        arround **index**.
)pydoc"
	);
}
