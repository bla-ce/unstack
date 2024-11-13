INC_DIR = inc
INCLUDES = $(wildcard $(INC_DIR)/*.inc)
TESTS = $(wildcard tests/*.s)

main: $(INCLUDES) main.s
	nasm -f elf64 -o main.o main.s -g -w+all -I$(INC_DIR)/
	ld -o main main.o

test: $(TESTS)
	@pass=0; fail=0; \
	for test in $(TESTS); do \
		echo -e "${YELLOW}Running $$test...${NC}"; \
		nasm -f elf64 -o "$${test%.s}.o" "$$test" -I$(INC_DIR)/; \
		ld -o "$${test%.s}" "$${test%.s}.o"; \
		"./$${test%.s}"; \
		ret=$$?; \
		if [ $$ret -eq 0 ]; then \
			echo -e "${GREEN}$$test passed.${NC}"; \
			pass=$$((pass + 1)); \
		else \
			echo -e "${RED}$$test failed with return code $$ret.${NC}"; \
			fail=$$((fail + 1)); \
		fi; \
		rm -f "$${test%.s}.o" "$${test%.s}"; \
	done; \
	echo -e "\n${YELLOW}Test Summary:${NC}"; \
	echo -e "${GREEN}Passed: $$pass${NC}"; \
	echo -e "${RED}Failed: $$fail${NC}"; \
	if [ $$fail -gt 0 ]; then exit 1; fi

