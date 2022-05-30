# Go parameters
GOBUILD=go build
GOCLEAN=go clean
GOTEST=go test
GOGET=go get
BINARY_NAME=integration_test
BINARY_UNIX=$(BINARY_NAME)_unix

# Make rule
.PHONY: all
all: build

.PHONY: build
build:
	$(GOBUILD)

.PHONY: clean
clean:
	$(GOCLEAN)

.PHONY: re
re: clean all

.PHONY: run
run: build
	./integration_test
# bash ./run.sh
