#include <linux/limits.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "args.h"


#define HOME_DIR getenv("HOME")
#define USAGE "Usage:\n" \
              "\tcpp_project <project_name> [flags]\n\n" \
              "Flags:\n" \
              "\t-h, --help\t\t: Outputs this message.\n" \
              "\t-n, --name\t\t: Sets the project binary's name.\n" \
              "\t-d, --description\t: Sets the project's description\n" \
              "\t-v, --version\t\t: Sets the project version.\n" \
              "\t-s, --shared\t\t: Makes the project a shared library.\n" \
              "\t-t, --template\t\t: Copies the template. If no template is given it'll output the available templates.\n"

int REQUIRED_PARAMS = 1;


typedef struct {
    char* project_name;
    char* name;
    char* version;
    char* description;
    char* template;
    char shared;
} Flags;

Flags flags = { .project_name = NULL, .name = NULL, .version = NULL, .description = NULL, .template = NULL, .shared = 0 };

void free_flags() {
    if(flags.project_name)
        free(flags.project_name);

    if(flags.version)
        free(flags.version);

    if(flags.template)
        free(flags.template);
}

void cleanup(char* project_directory, char* source_directory, char* include_directory, char* main_path, char* cmakelists_path) {
    free_flags();

    if(project_directory != NULL)
        free(project_directory);

    if(source_directory != NULL)
        free(source_directory);

    if(include_directory != NULL)
        free(include_directory);

    if(main_path != NULL)
        free(main_path);

    if(cmakelists_path != NULL)
        free(cmakelists_path);
}

char* join_paths(const char* path_1, const char* path_2) {
    size_t path_1_size = strlen(path_1) + 1;
    size_t path_2_size = strlen(path_2) + 1;

    size_t len = path_1_size + path_2_size;

    char* joined_path = calloc(1, len);

    if (joined_path == NULL) {
        fprintf(stderr, "Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

	char* tmp_path_1 = strdup(path_1);

    int path_1_offset = 0;
    int path_2_offset = 0;

    while(path_1[path_1_size - 2 - path_1_offset] == '/'
		||path_1[path_1_size - 2 - path_1_offset] == '\\') {
		tmp_path_1[path_1_size - 2 - path_1_offset] = '\0';
		len--;
        path_1_offset++;
    }

	while(path_2[path_2_offset] == '/'
		||path_2[path_2_offset] == '\\') {
		path_2_offset++;
		len--;
	}

    snprintf(joined_path, len, "%s/%s", tmp_path_1, path_2 + path_2_offset);

	free(tmp_path_1);

    return joined_path;
}

char* replace_substring(char* str, char* substring, char* with) {
	size_t str_len = strlen(str);
	size_t sub_len = strlen(substring);
	size_t with_len = strlen(with);

	int substring_count = 0;
	int i = 0;
	int new_i = 0;

	size_t new_len = (str_len - sub_len) + with_len + 1;
	char* new_str = calloc(1, new_len);
	if(new_str == NULL) {
		fprintf(stderr, "Failed to allocate memory.\n");
		return NULL;
	}

	while(i < str_len) {
		if(strncmp(str + i, substring, sub_len) == 0) {
			if(new_len < new_len + substring_count * (with_len - sub_len)) {
				new_len += (with_len - sub_len);

				char* new_ptr = realloc(new_str,new_len);
				if(new_ptr == NULL) {
					fprintf(stderr, "Failed to allocate.\n");
					return NULL;
				}

				new_str = new_ptr;
			}

			memcpy(new_str + new_i, with, with_len);

			substring_count++;

			new_i+=with_len;
			i+=sub_len;
		} else 
			new_str[new_i++] = str[i++];
	}

	new_str[new_i] = '\0';

	return new_str;
}

void create_config() {
    struct stat st = {0};
    char* config_dir = join_paths(HOME_DIR, ".config/cpp_project");
    if(stat(config_dir, &st) == -1) {
        if(mkdir(config_dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            free(config_dir);
            fprintf(stderr, "Failed to create config directory.\n");

            exit(EXIT_FAILURE);
        }

        char* template_dir = join_paths(config_dir, "templates");
        if(mkdir(template_dir, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
            free(config_dir);
            free(template_dir);
            fprintf(stderr, "Failed to create templates directory.\n");

            exit(EXIT_FAILURE);
        }

        free(template_dir);
    }
    free(config_dir);
}

void help_function(char* value) {
    printf("%s", USAGE);
}

void name_function(char* value) {
    if(value == NULL)
        return;

    flags.name = value;
}

void description_function(char* value) {
    if(value == NULL)
        return;

    flags.description = value;
}

void version_function(char* value) {
    if(value == NULL)
        return;

    flags.version = value;
}

void shared_function(char* value) {
    flags.shared = 1;
}

void template_function(char* value) {
    char* templates_path = join_paths(HOME_DIR, ".config/cpp_project/templates");
    printf("what: %s\n", templates_path);

    if(value == NULL) {
        DIR* templates_dir = opendir(templates_path);
        struct dirent *current_dir;
        if(!templates_dir)
            return;
        printf("Templates:\n");
        while((current_dir = readdir(templates_dir)) != NULL) {
            char* name = current_dir->d_name;
            if(*name == '.')
                continue;

            printf("\t%s\n", name);
        }
        closedir(templates_dir);
        return;
    }

    flags.template = join_paths(templates_path, value);
    printf("%s\n", flags.template);
    free(templates_path);
}

int main(int argc, char* argv[]) {
    create_config();

    if(argc == 1) {
        help_function(NULL);
        return EXIT_FAILURE;
    }

    FlagContainer* container = create_container();

    add_flag(container, "name", name_function, NULL);
    add_flag(container, "shared", shared_function, NULL);
    add_flag(container, "version", version_function, NULL);
    add_flag(container, "description", description_function, NULL);
    add_flag(container, "help", description_function, NULL);
    add_flag(container, "template", template_function, NULL);

    handle_args(container, argc, argv, REQUIRED_PARAMS);

    if(argc == REQUIRED_PARAMS + 1
    && *argv[1] == '-')
        return EXIT_FAILURE;
    
    flags.project_name = strdup(argv[1]);
    if(flags.project_name == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return EXIT_FAILURE;
    }

    if(flags.name == NULL)
        flags.name = flags.project_name;

    if(flags.version == NULL) {
        flags.version = strdup("1.0.0");
        if(flags.version == NULL) {
            free(flags.project_name);
            fprintf(stderr, "Failed to allocate memory.\n");
            return EXIT_FAILURE;
        }
    }

    printf("Project Information:\n");
    printf("\tName: %s\n", flags.name);
    printf("\tVersion: %s\n", flags.version);
    if(flags.description != NULL) {
        printf("\tDescription: %s\n", flags.description);
    } else
        flags.description = "";

    if(flags.template)
        printf("\tTemplate: %s\n", flags.template);
    printf("\tShared: %i\n", flags.shared);
    
    
    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL) {
        char* project_directory = join_paths(cwd, flags.project_name);
        struct stat st = {0};

        printf("\tDirectory: %s\n", project_directory);

        if(stat(project_directory, &st) == -1) {
            if(mkdir(project_directory, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
                fprintf(stderr, "Failed to create project directory.\n");
                cleanup(project_directory, NULL, NULL, NULL, NULL);

                return EXIT_FAILURE;
            }

            if(flags.template) {
                if(stat(flags.template, &st) == -1) {
                    fprintf(stderr, "Template \"%s\" doesn't exist.", flags.template);
                    cleanup(project_directory, NULL, NULL, NULL, NULL);
                    return EXIT_FAILURE;
                }

                char* command = (char*)malloc(strlen(project_directory) + strlen(flags.template) + 14);
                snprintf(command, strlen(project_directory) + strlen(flags.template) + 14, "cp -r \"%s\"/* \"%s\"", flags.template, project_directory);
                system(command);
                free(command);

                char* cmakelists_path = join_paths(project_directory, "CMakeLists.txt");
                if(stat(cmakelists_path, &st) == -1) {
                    cleanup(project_directory, NULL, NULL, NULL, cmakelists_path);
                    fprintf(stderr, "Template doesn't have a CMakeLists.txt");
                    return EXIT_FAILURE;
                }

                FILE* file = fopen(cmakelists_path, "r");
                if (!file) {
                    cleanup(project_directory, NULL, NULL, NULL, cmakelists_path);
                    fprintf(stderr, "Error opening file");
                    return EXIT_FAILURE;
                }

                fseek(file, 0L, SEEK_END);
                size_t size = ftell(file);
                rewind(file);

                char* cmakelists_data = calloc(1, size + 1);
                if (!cmakelists_data) {
                    cleanup(project_directory, NULL, NULL, NULL, cmakelists_path);
                    fprintf(stderr, "Memory allocation failed");
                    fclose(file);
                    return EXIT_FAILURE;
                }

                size_t read_size = fread(cmakelists_data, 1, size, file);
                if (read_size != size) {
                    cleanup(project_directory, NULL, NULL, NULL, cmakelists_path);
                    fprintf(stderr, "File read error");
                    free(cmakelists_data);
                    fclose(file);
                    return EXIT_FAILURE;
                }

                char* new_data = replace_substring(cmakelists_data, "__NAME__", flags.name);
                char* tmp_data = replace_substring(new_data, "__DESCRIPTION__", flags.description);
                free(new_data);
                new_data = tmp_data;
                tmp_data = replace_substring(new_data, "__VERSION__", flags.version);
                free(new_data);
                new_data = tmp_data;
                tmp_data = replace_substring(new_data, "__PROJECT_NAME__", flags.project_name);
                free(new_data);
                new_data = tmp_data;

                fclose(file);
                file = fopen(cmakelists_path, "w");
                fprintf(file, "%s", new_data);
                fclose(file);

                free(new_data);
                free(cmakelists_data);

                cleanup(project_directory, NULL, NULL, NULL, cmakelists_path);
                return EXIT_SUCCESS;
            }

            char* include_directory = join_paths(project_directory, "include/");
            if(stat(include_directory, &st) == -1) {
                if(mkdir(include_directory, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
                    fprintf(stderr, "Failed to create include directory.\n");
                    cleanup(project_directory, NULL, include_directory, NULL, NULL);

                    return EXIT_FAILURE;
                }
            }

            char* source_directory = join_paths(project_directory, "src/");
            if(stat(source_directory, &st) == -1) {
                if(mkdir(source_directory, S_IRWXU | S_IRWXG | S_IRWXO) == -1) {
                    fprintf(stderr, "Failed to create source directory.\n");
                    cleanup(project_directory, source_directory, include_directory, NULL, NULL);

                    return EXIT_FAILURE;
                }
            }

            FILE* cmakelists;
            char* cmakelists_path = join_paths(project_directory, "CMakeLists.txt");

            cmakelists = fopen(cmakelists_path, "w");
            if (cmakelists == NULL) {
                fprintf(stderr, "Failed to open CMakeLists.txt for writing.\n");
                cleanup(project_directory, source_directory, include_directory, NULL, cmakelists_path);
                return EXIT_FAILURE;
            }
            if(flags.shared == 0) {
                fprintf(cmakelists, "cmake_minimum_required(VERSION 3.10)\n\nproject(%s\n\t\tVERSION %s\n\t\tDESCRIPTION \"%s\"\n\t\tLANGUAGES CXX)\n\nset(CMAKE_CXX_STANDARD 17)\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\n\ninclude_directories(include)\n\nfile(GLOB_RECURSE SOURCE_FILES \"src/*.c*\")\nadd_executable(%s ${SOURCE_FILES})", flags.project_name, flags.version, flags.description, flags.name);
            } else {
                fprintf(cmakelists, "cmake_minimum_required(VERSION 3.10)\n\nproject(%s\n\t\tVERSION %s\n\t\tDESCRIPTION \"%s\"\n\t\tLANGUAGES CXX)\n\nset(CMAKE_CXX_STANDARD 17)\nset(CMAKE_CXX_STANDARD_REQUIRED ON)\n\ninclude_directories(include)\n\nfile(GLOB_RECURSE SOURCE_FILES \"src/*.c*\")\nadd_library(%s SHARED ${SOURCE_FILES})", flags.project_name, flags.version, flags.description, flags.name);
            }
            fclose(cmakelists);

            FILE* main_file;
            char* main_path = join_paths(source_directory, "main.cpp");

            main_file = fopen(main_path, "w");

            if (main_file == NULL) {
                fprintf(stderr, "Failed to open src/main.cpp for writing.\n");
                cleanup(project_directory, source_directory, include_directory, main_path, cmakelists_path);

                return EXIT_FAILURE;
            }

            fprintf(main_file, "#include <cstdio>\n\nint main(int argc, char* argv[]) {\n\tprintf(\"Hello, world!\\n\");\n}");
            fclose(main_file);

            cleanup(project_directory, source_directory, include_directory, main_path, cmakelists_path);
        } else {
            fprintf(stderr, "Project directory already exists.\n");
            cleanup(project_directory, NULL, NULL, NULL, NULL);

            return EXIT_FAILURE;
        }
    } else {
        free_flags();
        fprintf(stderr, "Couldn't get cwd.\n");
        return EXIT_FAILURE;
    }

}
