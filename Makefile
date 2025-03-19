PROJECT					= webserv
ROOT_DIR				= .
BUILD_DIR				= build
NAME					= ./$(BUILD_DIR)/$(PROJECT)
CMAKE_ARGS				= -DCMAKE_CXX_COMPILER=c++
CMAKE_BUILD_TYPE_ARG	?= Debug

MAKEFLAGS				+= --no-print-directory

CMAKE 					= cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE_ARG)

all: $(NAME)

cmake_build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) ..

$(NAME): .gen-compile_flags .gen-cmakelist cmake_build .init-tmp-dir
	@if [ ! -f $(PROJECT) ] || [ -n "$$(find CMakeLists.txt source include -newer $(PROJECT) -print -quit 2>/dev/null)" ]; then \
		$(MAKE) -C $(BUILD_DIR) $(MAKEFLAGS); \
		echo ""; \
		echo ${STY_GRE}${STY_BOL} "✅ Build Complete" ${STY_RES}; \
		echo ${STY_BLU} "➜ Project: " ${STY_WHI}${PROJECT}${STY_RES}; \
		echo ${STY_BLU} "➜ Status: " ${STY_GRE} "Mandatory" ${STY_RES}; \
	else \
		echo ${STY_YEL}"No changes detected. Nothing to do."${STY_RES}; \
	fi

.gen-cmakelist:
	@echo "Generating CMakeLists.txt..."
	@rm -f CMakeLists.txt
	@touch CMakeLists.txt

	@echo "cmake_minimum_required(VERSION 3.22.1)" >> CMakeLists.txt
	@echo "project(webserv LANGUAGES CXX VERSION 0.0.1)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "if(NOT WIN32)" >> CMakeLists.txt
	@echo "    string(ASCII 27 Esc)" >> CMakeLists.txt
	@echo "    set(RESET       \"\$${Esc}[m\")" >> CMakeLists.txt
	@echo "    set(BOLD        \"\$${Esc}[1m\")" >> CMakeLists.txt
	@echo "    set(RED         \"\$${Esc}[31m\")" >> CMakeLists.txt
	@echo "    set(GREEN       \"\$${Esc}[32m\")" >> CMakeLists.txt
	@echo "    set(YELLOW      \"\$${Esc}[33m\")" >> CMakeLists.txt
	@echo "    set(BLUE        \"\$${Esc}[34m\")" >> CMakeLists.txt
	@echo "    set(MAGENTA     \"\$${Esc}[35m\")" >> CMakeLists.txt
	@echo "    set(CYAN        \"\$${Esc}[36m\")" >> CMakeLists.txt
	@echo "    set(WHITE       \"\$${Esc}[37m\")" >> CMakeLists.txt
	@echo "    set(GRAY        \"\$${Esc}[90m\")" >> CMakeLists.txt
	@echo "endif()" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set(CMAKE_COLOR_MAKEFILE ON)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "function(debug_message)" >> CMakeLists.txt
	@echo "    message(STATUS \"\$${GRAY}\$${ARGV}\$${RESET}\")" >> CMakeLists.txt
	@echo "endfunction()" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)" >> CMakeLists.txt
	@echo "set(CMAKE_CXX_STANDARD 98)" >> CMakeLists.txt
	@echo "set(CMAKE_CXX_STANDARD_REQUIRED ON)" >> CMakeLists.txt
	@echo "set(CMAKE_CXX_EXTENSIONS OFF)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set(CMAKE_DEPENDS_IN_PROJECT_ONLY ON)" >> CMakeLists.txt
	@echo "set(CMAKE_CXX_DEPENDS_USE_COMPILER ON)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "add_compile_options(-Wall -Wextra -Werror)" >> CMakeLists.txt
	@echo "add_compile_options(\"$$<$$<CONFIG:Debug>:-gdwarf-4>\")" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set(BUILD_DIR \$${CMAKE_SOURCE_DIR}/build)" >> CMakeLists.txt
	@echo "set(SRC_DIR   \$${CMAKE_SOURCE_DIR}/source)" >> CMakeLists.txt
	@echo "set(INC_DIR   \$${CMAKE_SOURCE_DIR}/include)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "file(GLOB_RECURSE SOURCES \$${SRC_DIR}/*.cpp)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set(INCLUDE_FOLDERS \"\$${INC_DIR}\")" >> CMakeLists.txt
	@echo "file(GLOB_RECURSE ALL_PATHS \$${INC_DIR}/*)" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "foreach(path \$${ALL_PATHS})" >> CMakeLists.txt
	@echo "    get_filename_component(dir_path \$${path} DIRECTORY)" >> CMakeLists.txt
	@echo "    list(FIND INCLUDE_FOLDERS \$${dir_path} index)" >> CMakeLists.txt
	@echo "    if(\$${index} EQUAL -1)" >> CMakeLists.txt
	@echo "        list(APPEND INCLUDE_FOLDERS \$${dir_path})" >> CMakeLists.txt
	@echo "    endif()" >> CMakeLists.txt
	@echo "endforeach()" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "if(CMAKE_MESSAGE_LOG_LEVEL STREQUAL \"DEBUG\" OR CMAKE_MESSAGE_LOG_LEVEL STREQUAL \"TRACE\")" >> CMakeLists.txt
	@echo "    debug_message(\"Source directory: \$${SRC_DIR}\")" >> CMakeLists.txt
	@echo "    debug_message(\"Include directory: \$${INC_DIR}\")" >> CMakeLists.txt
	@echo "    debug_message(\"Include subdirectories:\")" >> CMakeLists.txt
	@echo "    foreach(folder \$${INCLUDE_FOLDERS})" >> CMakeLists.txt
	@echo "        debug_message(\"  \$${folder}\")" >> CMakeLists.txt
	@echo "    endforeach()" >> CMakeLists.txt
	@echo "endif()" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "add_executable(\$${PROJECT_NAME} \$${SOURCES})" >> CMakeLists.txt
	@echo "set_target_properties(\$${PROJECT_NAME} PROPERTIES" >> CMakeLists.txt
	@echo "    RUNTIME_OUTPUT_DIRECTORY \"\$${CMAKE_SOURCE_DIR}\")" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "set_property(TARGET \$${PROJECT_NAME} PROPERTY OBJECT_DEPENDS \"\$${HEADERS}\")" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "foreach(folder \$${INCLUDE_FOLDERS})" >> CMakeLists.txt
	@echo "    target_include_directories(\$${PROJECT_NAME} PUBLIC \$${folder})" >> CMakeLists.txt
	@echo "endforeach()" >> CMakeLists.txt
	@echo "" >> CMakeLists.txt

	@echo "target_link_libraries(\$${PROJECT_NAME} stdc++)" >> CMakeLists.txt


.gen-compile_flags:
	@echo "Generating compile_flags.txt..."
	@rm -f compile_flags.txt
	@touch compile_flags.txt
	@echo "-Wall" >> compile_flags.txt
	@echo "-Wextra" >> compile_flags.txt
	@echo "-Werror" >> compile_flags.txt
	@echo "-std=c++98" >> compile_flags.txt
	@echo "-Iinclude" >> compile_flags.txt
	@echo "-Iinclude/config" >> compile_flags.txt
	@echo "-Iinclude/shell" >> compile_flags.txt
	@echo "-Iinclude/kernel" >> compile_flags.txt
	@echo "-Iinclude/templates" >> compile_flags.txt
	@echo -n "-Iinclude/utils" >> compile_flags.txt

.init-tmp-dir:
	echo "Creating tmp directory..."
	@mkdir -p $(ROOT_DIR)/tmp
	echo "Creating upload directory..."
	@mkdir -p $(ROOT_DIR)/www/upload

# bonus: cmake_build
# 	@$(MAKE) -C $(BUILD_DIR) -j8
# 	@echo ""
# 	@echo ${STY_GRE}${STY_BOL} "🚀 Build Complete" ${STY_RES}
# 	@echo ${STY_BLU} "➜ Project: " ${STY_WHI}${PROJECT}${STY_RES}
# 	@echo ${STY_BLU} "➜ Status: " ${STY_GRE} "Bonus" ${STY_RES}

bonus:
	@echo $(STY_YEL)"[WARNING] Bonus part is not implemented yet"$(STY_RES)

clean:
	@rm -rf $(BUILD_DIR)
	@echo $(STY_BLU)"[INFO] Clean the build directory"$(STY_RES)

fclean: clean
	@rm -f $(PROJECT)
	@rm -f $(BUILD_DIR)/CMakeCache.txt
	@echo $(STY_BLU)"[INFO] Clean the tmp folder"$(STY_RES)
	@rm -rf $(ROOT_DIR)/tmp
	@rm -rf $(ROOT_DIR)/www/upload
	@rm -f CMakeLists.txt
	@rm -f compile_flags.txt
	@echo $(STY_BLU)"[INFO] Removed CMakeLists.txt."$(STY_RES)
	@echo $(STY_BLU)"[INFO] Perform full clean"$(STY_RES)

re: fclean
	@sleep 1
	@$(MAKE) all

.PHONY: all clean fclean re cmake_build bonus .init-tmp-dir .gen-cmakelist .gen-compile_flags

# **************************** COLORS ******************************* #

STY_RES			:=	"\e[0m"
STY_BOL			:=	"\e[1m"
STY_ITA			:=	"\e[3m"
STY_UND			:=	"\e[4m"
STY_RED			:=	"\e[31m"
STY_GRE			:=	"\e[32m"
STY_YEL			:=	"\e[33m"
STY_BLU			:=	"\e[34m"
STY_MAG			:=	"\e[35m"
STY_CYA			:=	"\e[36m"
STY_WHI			:=	"\e[37m"
STY_GRA			:=	"\e[90m"
STY_WHI_BRI		:=	"\e[97m"
STY_BLA_BG		:=	"\e[40m"
STY_RED_BG		:=	"\e[41m"
STY_GRE_BG		:=	"\e[42m"
STY_YEL_BG		:=	"\e[43m"
STY_BLU_BG		:=	"\e[44m"
STY_MAG_BG		:=	"\e[45m"
STY_CYA_BG		:=	"\e[46m"
STY_WHI_BG		:=	"\e[47m"
STY_GRA_BG		:=	"\e[100m"
STY_WHI_BRI_BG	:=	"\e[107m"

# *************************** MAKEFILE DEBUGGING ***************************** #

#	Execute "make print-[variable name]" to list the variable's values

print-%			:
					echo $* = $($*)
