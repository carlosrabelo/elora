MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build check clean fmt help lint quality run setup test

help: ## Show available targets
	@echo "elora - Available targets"
	@echo ""
	@grep -hE '^[a-zA-Z_-]+:.*## ' $(MAKEFILE_LIST) \
		| sort \
		| awk 'BEGIN {FS = ":.*## "} {printf "  %-15s %s\n", $$1, $$2}'

setup: ## Download dependencies
	@../.make/setup.sh

build: ## Compile binary
	@../.make/build.sh

run: ## Run binary
	@../.make/run.sh

test: ## Compile and run tests
	@../.make/test.sh

check: ## Syntax-check all source files
	@../.make/check.sh

lint: ## Run cppcheck
	@../.make/lint.sh

fmt: ## Format source
	@../.make/fmt.sh

quality: ## All quality checks
	@../.make/quality.sh

clean: ## Remove build artifacts
	@rm -rf ../bin/
