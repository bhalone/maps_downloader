set(BIN_DIR $ENV{SolutionDir}/binaries)


add_custom_target(CopyFiles ALL
    COMMAND xcopy /Y /I ${BIN_DIR} ${CMAKE_CURRENT_BINARY_DIR}
)