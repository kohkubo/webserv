# Go parameters
GOBUILD=go mod tidy && go build
GOCLEAN=go clean
GOTEST=go test
GOGET=go get
BINARY_NAME=integrationtest
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
	./integrationtest

.PHONY: run_all
run_all: build
	./integrationtest -all

.PHONY: gotest
gotest: build
	go test ./...
