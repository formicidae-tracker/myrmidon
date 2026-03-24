if(NOT DEFINED STUB_FILE)
	message(FATAL_ERROR "STUB_FILE is not defined")
endif()
file(READ "${STUB_FILE}" _content)

string(REGEX REPLACE "\n__version__[^\n]*" "\n__version__: str" _content
					 "${_content}"
)

file(WRITE "${STUB_FILE}" "${_content}")
