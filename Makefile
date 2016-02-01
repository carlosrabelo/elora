MAKEFLAGS += --no-print-directory

.DEFAULT_GOAL := help

.PHONY: build check clean fmt help lint quality run setup test

help: ## Show available targets
	@$(MAKE) -C elora help

setup: ## Download dependencies (e.g. Catch2 header)
	@$(MAKE) -C elora setup

build: ## Compile binary
	@$(MAKE) -C elora build

run: ## Run binary
	@$(MAKE) -C elora run

test: ## Build and run tests
	@$(MAKE) -C elora test

check: ## Syntax-check sources
	@$(MAKE) -C elora check

lint: ## Run cppcheck
	@$(MAKE) -C elora lint

fmt: ## Format with clang-format
	@$(MAKE) -C elora fmt

quality: ## fmt + lint + test
	@$(MAKE) -C elora quality

clean: ## Remove build artifacts
	@$(MAKE) -C elora clean
