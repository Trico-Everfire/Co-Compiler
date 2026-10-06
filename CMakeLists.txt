cmake_minimum_required(VERSION 3.26)
project(cocompiler)

set(CMAKE_PREFIX_PATH "/media/trico/Storage Drive X2/Qt/6.5.3/gcc_64/lib/cmake/")

set(QT_BASEDIR "/media/trico/Storage Drive X2/Qt/6.8.0/gcc_64/")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC
    src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h

        src/widgets/qwcfiledialog.h
        src/widgets/qwcfiledialog.cpp
        res/resource.qrc
        src/widgets/processlistwidgetitem.cpp
        src/widgets/processlistwidgetitem.h
        src/confighandler.cpp
        src/confighandler.h
        src/enums.h
        src/widgets/console.cpp
        src/widgets/console.h
        src/widgets/variableinputdialog.cpp
        src/widgets/variableinputdialog.h
        src/widgets/editconfigurations.cpp
        src/widgets/editconfigurations.h
        src/widgets/luasyntaxhighlighting.cpp
        src/widgets/luasyntaxhighlighting.h
)

set(LUA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lua)

add_library(lua STATIC
        ${LUA_DIR}/lapi.c
        ${LUA_DIR}/lauxlib.c
        ${LUA_DIR}/lbaselib.c
        ${LUA_DIR}/lcode.c
        ${LUA_DIR}/lcorolib.c
        ${LUA_DIR}/lctype.c
        ${LUA_DIR}/ldblib.c
        ${LUA_DIR}/ldebug.c
        ${LUA_DIR}/ldo.c
        ${LUA_DIR}/ldump.c
        ${LUA_DIR}/lfunc.c
        ${LUA_DIR}/lgc.c
        ${LUA_DIR}/linit.c
        ${LUA_DIR}/liolib.c
        ${LUA_DIR}/llex.c
        ${LUA_DIR}/lmathlib.c
        ${LUA_DIR}/lmem.c
        ${LUA_DIR}/loadlib.c
        ${LUA_DIR}/lobject.c
        ${LUA_DIR}/lopcodes.c
        ${LUA_DIR}/loslib.c
        ${LUA_DIR}/lparser.c
        ${LUA_DIR}/lstate.c
        ${LUA_DIR}/lstring.c
        ${LUA_DIR}/lstrlib.c
        ${LUA_DIR}/ltable.c
        ${LUA_DIR}/ltablib.c
        ${LUA_DIR}/ltm.c
        ${LUA_DIR}/lua.c
        ${LUA_DIR}/lundump.c
        ${LUA_DIR}/lutf8lib.c
        ${LUA_DIR}/lvm.c
        ${LUA_DIR}/lzio.c
)

add_library(sol STATIC
        thirdparty/sol/sol.hpp
        thirdparty/sol/config.hpp
        thirdparty/sol/forward.hpp)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        DBus
        REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/speedykeyv")

add_executable(cocompiler ${SRC})

target_include_directories(cocompiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/ PUBLIC ${LUA_DIR})

target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::DBus
        keyvalues
        lua
        sol
)

configure_file("${QT_BASEDIR}/lib/libQt6Core${QT_LIB_SUFFIX}.so.6" "${CMAKE_BINARY_DIR}/libQt6Core${QT_LIB_SUFFIX}.so.6" COPYONLY)
configure_file("${QT_BASEDIR}/lib/libQt6Gui${QT_LIB_SUFFIX}.so.6" "${CMAKE_BINARY_DIR}/libQt6Gui${QT_LIB_SUFFIX}.so.6" COPYONLY)
configure_file("${QT_BASEDIR}/lib/libQt6Widgets${QT_LIB_SUFFIX}.so.6" "${CMAKE_BINARY_DIR}/libQt6Widgets${QT_LIB_SUFFIX}.so.6" COPYONLY)

configure_file("${QT_BASEDIR}/lib/libicui18n.so.73.2" "${CMAKE_BINARY_DIR}/libicui18n.so.73" COPYONLY)
configure_file("${QT_BASEDIR}/lib/libicuuc.so.73.2" "${CMAKE_BINARY_DIR}/libicuuc.so.73" COPYONLY)
configure_file("${QT_BASEDIR}/lib/libicudata.so.73.2" "${CMAKE_BINARY_DIR}/libicudata.so.73" COPYONLY)

configure_file("${QT_BASEDIR}/lib/libQt6DBus${QT_LIB_SUFFIX}.so.6" "${CMAKE_BINARY_DIR}/libQt6DBus${QT_LIB_SUFFIX}.so.6" COPYONLY)

configure_file("${QT_BASEDIR}/plugins/platforms/libqminimal.so" "${CMAKE_BINARY_DIR}/platforms/libqminimal.so" COPYONLY)
configure_file("${QT_BASEDIR}/plugins/platforms/libqxcb.so" "${CMAKE_BINARY_DIR}/platforms/libqxcb.so" COPYONLY)

configure_file("${QT_BASEDIR}/plugins/platforms/libqminimal.so" "${CMAKE_BINARY_DIR}/platforms/libqminimal.so" COPYONLY)

file(GLOB ${PROJECT_NAME}_QT_PLUGINS_XCBGLINTEGRATIONS "${QT_BASEDIR}/plugins/xcbglintegrations/*.so*")
file(COPY ${${PROJECT_NAME}_QT_PLUGINS_XCBGLINTEGRATIONS} DESTINATION "${CMAKE_BINARY_DIR}/xcbglintegrations")