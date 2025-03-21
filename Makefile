PROJECT					= webserv
ROOT_DIR				= .
BUILD_DIR				= build
NAME					= ./$(BUILD_DIR)/$(PROJECT)
CMAKE_ARGS				= -DCMAKE_CXX_COMPILER=c++
CMAKE_BUILD_TYPE_ARG	?= Debug

MAKEFLAGS				+= --no-print-directory -j

CMAKE 					= cmake $(CMAKE_ARGS) -DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE_ARG)

all: $(NAME)

cmake_build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) ..

$(NAME): cmake_build .init-tmp-dir
	@if [ ! -f $(PROJECT) ] || [ -n "$$(find CMakeLists.txt source include -newer $(PROJECT) -print -quit 2>/dev/null)" ]; then \
		$(MAKE) -C $(BUILD_DIR) $(MAKEFLAGS); \
		echo ""; \
		echo ${STY_GRE}${STY_BOL} "✅ Build Complete" ${STY_RES}; \
		echo ${STY_BLU} "➜ Project: " ${STY_WHI}${PROJECT}${STY_RES}; \
		echo ${STY_BLU} "➜ Status: " ${STY_GRE} "Mandatory" ${STY_RES}; \
	else \
		echo ${STY_YEL}"No changes detected. Nothing to do."${STY_RES}; \
	fi

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
	@echo $(STY_BLU)"[INFO] Perform full clean"$(STY_RES)

re: fclean
	@sleep 1
	@$(MAKE) all

.PHONY: all clean fclean re cmake_build bonus

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
