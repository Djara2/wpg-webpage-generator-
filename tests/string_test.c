#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../wpgstring.h"

enum ParameterSetType { 
	TYPE_NONE,
	TYPE_STRING_CREATE,
	TYPE_STRING_SPLICE
};

enum TestEnvironmentType {
	ENVIRONMENT_NONE,
	ENVIRONMENT_STRING_CREATE,
	ENVIRONMENT_STRING_SPLICE
};

struct StringCreateTestParameters { 
	char *text;
	size_t length;
}; 

struct StringSpliceTestParameters {
	unsigned short start;
	unsigned short end;
	unsigned short step;
	struct String *base;
	struct String *expected_output;
};

struct TestEnvironment {
	enum TestEnvironmentType type;
	void *parameters;
	size_t parameters_length;
	bool *results;
	size_t results_length;
};

// Utility functions
void run_and_evaluate_tests(char *function_title, 
		            bool (*test_function)(void*),
			    void *parameter_sets,
			    enum ParameterSetType parameter_set_type, 
			    bool *results,
			    size_t results_length);
bool are_tests_passed(bool *results, size_t length);

// Test functions
bool test_string_init();		        	// Simple init test
bool test_string_create(void *parameters);		// String with default value passed in
bool test_string_set(void *parameters);			// Recently init'd string (no value) having a value set  
bool test_string_splice(void *parameters);

struct TestEnvironment* test_environment_create(enum TestEnvironmentType type) {
	// Create TestEnvironment
	struct TestEnvironment *environment = malloc(sizeof(struct TestEnvironment));
	if (environment == NULL) { 
		fprintf(stderr, "[test_environment_create] Failed to allocate space for TestEnvironment struct on the heap.\n");
		return NULL;
	}
	environment->type = type;
	
	// Behavior based on type of environment
	switch (type) { 
		case ENVIRONMENT_NONE:
			fprintf(stderr, "[test_environment_create] ERROR: TestEnvironmentType code %d (ENVIRONMENT_NONE) is invalid.\n", type);
			free(environment);
			return NULL;
			break;

		case ENVIRONMENT_STRING_CREATE:
			break;

		case ENVIRONMENT_STRING_SPLICE:
			// Allocate space for the StringSpliceTestParameters
			environment->parameters = (void*) malloc(sizeof(struct StringSpliceTestParameters) * 3);
			if (environment->parameters == NULL) {
				fprintf(stderr, "[test_environment_create] Failed to allocate memory for 3 StringSpliceTestParameters on the heap.\n");
				free(environment);
				return NULL;
			}

			// Set the values for the individual parameters.
			struct StringSpliceTestParameters *parameters = (struct StringSpliceTestParameters*) environment->parameters;
			// (1) Set the values for the first parameter set
			parameters[0]->base = string_create("My name is Dave and I am a programmer.");
			parameters[1]->base = 
			break;

		default:
			fprintf(stderr, "[test_environment_create] ERROR: TestEnvironmentType code %d is invalid/unimplemented.\n", type);
			free(environment);
			return NULL;
			break; 
	} 
}

int main(int argc, char **argv) {
	// Test simple string init
	bool string_init_test_results[1];
	run_and_evaluate_tests("string_init", &test_string_init, NULL, TYPE_NONE, string_init_test_results, 1);
	
	// Test string creation with default value
	bool string_create_test_results[2];
	char *longer_string = "This is some text that I believe will be longer than the default 32 characters that a string is initially given.";
	size_t longer_string_length = strlen(longer_string);
	struct StringCreateTestParameters string_create_test_parameters[2];
	string_create_test_parameters[0].text = "dave";
	string_create_test_parameters[0].length = 4;

	string_create_test_parameters[1].text = longer_string;
	string_create_test_parameters[1].length = longer_string_length;
	struct StringCreateTestParameters *current_parameters;
	run_and_evaluate_tests(
		"string_create",
		&test_string_create,
		(void*) string_create_test_parameters,
		TYPE_STRING_CREATE,
		string_create_test_results,
		2
	);
	
	// Test string_set with a recently initialized string
	bool string_set_test_results[2];
	run_and_evaluate_tests("string_set", &test_string_set, (void*) string_create_test_parameters, TYPE_STRING_CREATE, string_set_test_results, 2);

	// Test string splice
	run_and_evaluate_tests("string_splice", &test_string_splice, (void*) string_splice_test_parameters, TYPE_STRING_SPLICE, string_splice_test_results, 3);

	return 0;
} // end main

void run_and_evaluate_tests(char *function_title, 
		            bool (*test_function)(void*),
			    void *parameter_sets,
			    enum ParameterSetType parameter_set_type, 
			    bool *results,
			    size_t results_length) { 
	printf("Running tests for the %s() function...\n", function_title);

	// Run function with provided parameter sets and store results 
	void *current_parameters;
	for (size_t i = 0; i < results_length; i++) {
		switch (parameter_set_type) {
			case TYPE_NONE:
				current_parameters = NULL;
				break;
			
			case TYPE_STRING_CREATE:
				current_parameters = &((struct StringCreateTestParameters*) parameter_sets)[i];
				break;

			case TYPE_STRING_SPLICE:
				current_parameters = &((struct StringSpliceTestParameters*) parameter_sets)[i];
				break;

			default:
				current_parameters = NULL;
				break;
		} 

		printf("Running test %zu/%zu...\n", i + 1, results_length);
		results[i] = test_function(current_parameters);
	} 

	// Evaluate performance
	if (are_tests_passed(results, results_length) == false)
		fprintf(stderr, "Failed on some tests of the %s() function.\n", function_title);

	printf("\n");
}

bool are_tests_passed(bool *results, size_t length) {
	size_t tests_passed = 0;	
	if (results == NULL) {
		fprintf(stderr, "[are_tests_passed] Cannot check validity of test results using a NULL pointer in place of a boolean pointer.\n");
	}

	for (size_t i = 0; i < length; i++) {
		if (results[i] == false) { 
			fprintf(stderr, "FAILED: test #%zu.\n", i + 1);
			continue;
		} 

		printf("PASSED: Test #%zu.\n", i + 1);
		tests_passed++;
	}

	printf("Passed %zu/%zu tests.\n", tests_passed, length);
	if (tests_passed == length) return true;
	else                        return false;
}

bool test_string_init() {
	struct String *string = string_init();
	if (string == NULL) { 
		fprintf(stderr, "[test_string_simple_creation] Call to string_init() returned NULL pointer.\n");
		return false;
	} 

	if (string->length != 0) {
		fprintf(stderr, "[test_string_simple_creation] Newly created string did not have 0 length.\n");
		return false;
	}

	if (string->capacity != 32) {
		fprintf(stderr, "[test_string_simple_creation] Newly created string did not have a capacity of 32.\n");
		return false;
	}

	free(string->data);
	free(string);
	return true;
}

bool test_string_create(void *parameters) {
	char   *text  = ((struct StringCreateTestParameters*) parameters)->text;
	size_t length = ((struct StringCreateTestParameters*) parameters)->length;

	if (text == NULL) {
		fprintf(stderr, "[test_string_create] Provided test string was NULL.\n");
		return false;
	}
	
	struct String *string = string_create(text, (unsigned short) length);
	if (string == NULL) {
		fprintf(stderr, "[test_string_create] Returned string was NULL.\n");
		return false;
	}

	if (string->data == NULL) {
		fprintf(stderr, "[test_string_create] Returned string had a NULL pointer for its data buffer.\n");
		free(string);
		return false;
	} 

	if (string->length != length) {
		fprintf(stderr, "[test_string_create] Returned string has a different length attribute (%hu) than the value provided (%zu).\n", string->length, length);
		free(string->data);
		free(string);
		return false; 
	}

	if (strcmp(string->data, text) != 0) {
		fprintf(stderr, "[test_string_create] Returned string does not have the expected textual contents (\"%s\" != \"%s\").\n", string->data, text);
		free(string->data);
		free(string);
		return false;
	}

	return true;
}

bool test_string_set(void *parameters) { 
	char *text = ((struct StringCreateTestParameters*) parameters)->text;
	size_t length = ((struct StringCreateTestParameters*) parameters)->length;

	if (text == NULL) { 
		fprintf(stderr, "[test_string_set] Cannot perform a test using a test value char pointer that points to NULL.\n");
		return false;
	}

	struct String *string = string_init();
	if (string == NULL) { 
		fprintf(stderr, "[test_string_set] Failed to allocate space for a default init string on the heap.\n");
		return false;
	}

	if (string_set(string, text, length) != STRING_ERROR_NONE) { 
		fprintf(stderr, "[test_string_set] Call to string_set failed while using string \"%s\" of length %zu.\n", text, length);
		if (string->data != NULL) free(string->data);
		if (string != NULL)       free(string);
		return false;
	}

	return true;
}

bool test_string_splice(void *parameters) {
	if (parameters == NULL) { 
		fprintf(stderr, "[test_string_splice] Cannot use NULL pointer for parameters argument.\n");
		return false;
	}

	struct String *string = ((struct StringSpliceTestParameters*) parameters)->base;
	unsigned short start = ((struct StringSpliceTestParameters*) parameters)->start;
	unsigned short end = ((struct StringSpliceTestParameters*) parameters)->end;
	unsigned short step = ((struct StringSpliceTestParameters*) parameters)->step;
	struct String *expected_output = ((struct StringSpliceTestParameters*) parameters)->expected_output;

	struct String *spliced_string = string_splice(string, start, end, step);
	if (spliced_string == NULL) {
		fprintf(stderr, "[test_string_splice] Output string is a NULL pointer.\n");
		return false;
	}
	
	if (spliced_string->data == NULL) { 
		fprintf(stderr, "[test_string_splice] Output string has a NULL pointer for its data attribute.\n");
		free(spliced_string);
		return false;
	}

	if (spliced_string->length != expected_output->length) {
		fprintf(stderr, "[test_string_splice] Output string does not have the same length (got %hu, but expected  %hu) attribute as the expected output string.\n", spliced_string->length, expected_output->length);
		free(spliced_string->data);
		free(spliced_string);
		return false;
	}

	if (strcmp(spliced_string->data, expected_output->data) != 0) {
		fprintf(stderr, "[test_string_splice] Output string does not have the same textual contents (got \"%s\", but expected  \"%s\") attribute as the expected output string.\n", spliced_string->data, expected_output->data);
		free(spliced_string->data);
		free(spliced_string);
		return false;
	}

	free(spliced_string->data);
	free(spliced_string);
	return true;
}
