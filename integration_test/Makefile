# Go parameters
GOCMD=go
GOBUILD=$(GOCMD) build
GOCLEAN=$(GOCMD) clean
GOTEST=$(GOCMD) test
GOGET=$(GOCMD) get
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
	./$(BINARY_NAME)

.PHONY: doc
doc:
	godoc -http=:6060

#go doc -all converter