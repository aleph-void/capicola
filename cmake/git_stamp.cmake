# Writes OUT as a one-line header defining CAPICOLA_GIT_HASH: the short hash,
# with "-dirty" when tracked files differ from HEAD. Runs on every build, so the
# stamp HostLink reports can't go stale between configures. Modified content
# inside submodules is ignored (libDaisy's build leaves the SDK "dirty").
execute_process(COMMAND git rev-parse --short HEAD
                WORKING_DIRECTORY "${SRC}"
                OUTPUT_VARIABLE hash
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_QUIET)
if(NOT hash)
    set(hash "nogit")
else()
    execute_process(COMMAND git diff --quiet --ignore-submodules=dirty HEAD
                    WORKING_DIRECTORY "${SRC}"
                    RESULT_VARIABLE dirty)
    if(dirty)
        set(hash "${hash}-dirty")
    endif()
endif()
set(content "#define CAPICOLA_GIT_HASH \"${hash}\"\n")
if(EXISTS "${OUT}")
    file(READ "${OUT}" old)
endif()
if(NOT old STREQUAL content)
    file(WRITE "${OUT}" "${content}")
endif()
