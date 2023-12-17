cmake_minimum_required(VERSION 3.26)
project(cocompiler)

set(CMAKE_PREFIX_PATH "/media/trico/Storage Drive X2/Qt/6.5.3/gcc_64/lib/cmake/")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(SRC
    src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h

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
        src/cocoscriptparser.cpp
        src/cocoscriptparser.h
)



find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/speedykeyv")

add_executable(cocompiler ${SRC})

target_link_libraries(${PROJECT_NAME}
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        keyvalues
)
