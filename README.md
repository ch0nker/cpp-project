# CPP Project

A tool designed for personal use to simplify the creation of C++ projects.

## Table of Contents

- [Build](#build)
- [Usage](#usage)
- [Flags](#flags)

## Build

To build the project, follow these steps:

1. Create a build directory and navigate into it:

   ```bash
   mkdir build
   cd build
   ```

2. Run CMake and make:
   ```bash
   cmake ..
   make
   ```

Alternatively, you can use the installation script:

```bash
./install.sh
```

## Templates

In order to create a template you'll need to either run `cpp_project` at least once with or without any arguments or run the following commands.

```bash
cd ~/.config
mkdir -p ./cpp_project/templates
cd ./cpp_project/templates
mkdir example_template
```

Then you'd have to create a CMakeLists.txt file within the newly created directory using these placeholders:
- `__PROJECT_NAME__`: Placeholder for the first argument used.
- `__NAME__`: Placeholder for either the first argument or the name flag.
- `__VERSION__`: Placeholder for the version flag.
- `__DESCRIPTION__`: Placeholder for the description flag.

### Example CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.10)

project(__PROJECT_NAME__
        VERSION __VERSION__
        DESCRIPTION "__DESCRIPTION__")

file(GLOB CXX_SOURCES "src/*.c*")

add_executable(__NAME__ ${CXX_SOURCES})
```


## Usage

To create a new project, use the following command:

```bash
cpp_project <project_name> [flags]
```

### Flags

The following flags are available:

- `-h`, `--help`: Outputs a help message.
- `-n`, `--name`: Sets the project binary's name.
- `-d`, `--description`: Sets the project's description.
- `-v`, `--version`: Sets the project version.
- `-s`, `--shared`: Makes the project a shared library.
- `-t`, `--template`: Copies the template. If no template is given it'll output the available templates.

## Example

Here’s an example of how to create a new project:

```bash
cpp_project MyAwesomeProject -n "project" -d "This is an awesome project!" -v "1.0.0"
```

This is the expected output from the example given above:

```
Project Information:
        Name: project
        Verison: 1.0.0
        Description: This is an awesome project!
        Shared: 0
        Directory: ./MyAwesomeProject
```
