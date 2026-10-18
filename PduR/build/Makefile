include config.mk
.DEFAULT_GOAL := all
OUT := out
GENERATED := ../PduR/src/PduR_PBcfg.c
SOURCES := $(wildcard ../Com/src/*.c ../CanIf/src/*.c ../PduR/src/*.c)
OBJECTS := $(patsubst ../%.c,$(OUT)/%.o,$(SOURCES))
.PHONY: all generate demo ui test clean
all: $(OUT)/bin/demo
# Regenerate on every build so switching CONFIG cannot leave stale tables.
.PHONY: FORCE
$(GENERATED): FORCE
	$(PYTHON) ../tools/generate.py $(CONFIG) $@
generate: $(GENERATED)
$(OUT)/%.o: ../%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@
$(OUT)/bin/demo: $(OBJECTS) $(OUT)/demo/main.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
$(OUT)/test_stack: $(OBJECTS) $(OUT)/tests/test_stack.o
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@
demo: $(OUT)/bin/demo
	./$(OUT)/bin/demo
ui: $(OUT)/bin/demo
	$(PYTHON) ../demo/serve.py --port $(PORT)
test: $(OUT)/test_stack
	./$(OUT)/test_stack
	$(PYTHON) ../tests/test_generator.py
clean:
	$(RM) -r $(OUT)
-include $(OBJECTS:.o=.d) $(OUT)/demo/main.d $(OUT)/tests/test_stack.d
