OUTPUT = .output

$(OUTPUT)/address : *.cpp $(wildcard %.h) | $(OUTPUT)
	g++ -g -O2 --std=c++17 $^ -lelf -o $@

$(OUTPUT):
	mkdir -p $(OUTPUT)

.PHONY: clean
clean:
	rm -rf .output
