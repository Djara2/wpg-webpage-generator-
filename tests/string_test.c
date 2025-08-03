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
	struct String base;
	struct String expected_output;
};

void string_splice_test_parameters_set_range(struct StringSpliceTestParameters *parameter, unsigned short start, unsigned short end, unsigned short step) {
	if (parameter == NULL) {
		fprintf(stderr, "[string_splice_test_parameters_set_range] Cannot set attributes in a memory region pointed to by a NULL pointer.\n");
		return;
	} 

	parameter->start = start;
	parameter->end = end;
	parameter->step = step;
	return; 
}

struct TestEnvironment {
	enum TestEnvironmentType type;
	void *parameters;
	size_t parameters_length;
	bool *results;
	size_t results_length;
};


void test_parameter_destroy(void *test_parameter, enum ParameterSetType parameter_type) {
	if (test_parameter == NULL) {
		fprintf(stderr, "[test_parameter_destroy] Cannot free the memory addressed by a NULL pointer.\n");
		return;
	} 

	switch (parameter_type) {
		case TYPE_NONE:
			fprintf(stderr, "[test_parameter_destroy] Cannot free the memory of a parameter set without knowing which kind of parameter it is (code %d \"TYPE_NONE\" is invalid).\n", parameter_type);
			return;
			break; 

		case TYPE_STRING_CREATE:
			struct StringCreateTestParameters *parameter = (struct StringCreateTestParameters*) test_parameter;
			if (parameter->text != NULL) free(parameter->text); 
			free(parameter);
			return;
			break;

		case TYPE_STRING_SPLICE:
			struct StringSpliceTestParameters *parameter = (struct StringSpliceTestParameters*) test_parameter;
			// Free the input string
			if (parameter->base != NULL && (parameter->base).data != NULL)
				free( (parameter->base).data ); 

			// Free the expected output string
			if (parameter->expected_output != NULL && (parameter->expected_output).data != NULL) 
				free( (parameter->expected_output).data );
			
			
			// If using an array of parameters, then we don't want to free this one yet.
			free(parameter);
			return;
			break;

		default:  
			fprintf(stderr, "[test_parameter_destroy] Code %d is invalid or unimplemented.\n", parameter_type);
			return;
			break;
	}

	return;
}

void test_parameter_array_destroy(void *test_parameters, enum ParameterSetType parameter_type, size_t length) {
	if (test_parameters == NULL) {
		fprintf(stderr, "[test_parameter_array_destroy] Cannot free the memory addressed by a NULL pointer.\n");
		return;
	} 

	switch (parameter_type) {
		case TYPE_NONE:
			fprintf(stderr, "[test_parameter_array_destroy] Cannot free the memory of a parameter set without knowing which kind of parameter it is (code %d \"TYPE_NONE\" is invalid).\n", parameter_type);
			return;
			break; 

		case TYPE_STRING_CREATE:
			struct StringCreateTestParameters *parameters = (struct StringCreateTestParameters*) test_parameters;
			struct StringCreateTestParameters *current_parameter;
			// Free buffers within individual parameter structs 
			for (size_t i = 0; i < length; i++) {
				current_parameter = parameters[i];
				if (current_parameter->text != NULL) 
					free(parameter->text); 
			}
			// Free the entire array
			free(parameters);
			return;
			break;

		case TYPE_STRING_SPLICE:
			struct StringSpliceTestParameters *parameters = (struct StringSpliceTestParameters*) test_parameters;
			struct StringSpliceTestParameters *current_parameter;
			char *current_base;
			char *current_expected_output;
			// Free the buffers within individual parameter structs
			for(size_t i = 0; i < length; i++) { 
				// Set current items of interest
				current_parameter = parameters[i]; 
				current_base = current_parameter->base;
				current_expected_output = current_parameter->expected_output;

				// Free the input string
				if (current_base != NULL && current_base.data != NULL)
					free(current_base.data); 

				// Free the expected output string
				if (current_expected_output != NULL && current_expected_output.data != NULL) 
					free(current_expected_output.data);
			}
			
			// Free the array itself
			free(parameters);
			return;
			break;

		default:  
			fprintf(stderr, "[test_parameter_array_destroy] Code %d is invalid or unimplemented.\n", parameter_type);
			return;
			break;
	}

	return;
}

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
			environment->parameters_length = 3;
			// Allocate space for the StringSpliceTestParameters
			environment->parameters = (void*) malloc(sizeof(struct StringSpliceTestParameters) * 3);
			if (environment->parameters == NULL) {
				fprintf(stderr, "[test_environment_create] Failed to allocate memory for 3 StringSpliceTestParameters on the heap.\n");
				free(environment);
				return NULL;
			}
			environment->results = malloc(sizeof(bool) * 3);
			if (environment->results == NULL) { 
				fprintf(stderr, "[test_environment_create] Failed to allocate memory for a size-3 boolean array on the heap.\n");
				free(environment->parameters);
				free(environment);
				return NULL;
			}

			// SET THE VALUES FOR THE INDIVIDUAL PARAMETERS.
			struct StringSpliceTestParameters *parameters = (struct StringSpliceTestParameters*) environment->parameters;
			// (1.1) Set the ranges for the first parameter set
			string_splice_test_parameters_set_range(parameters[0], 0, 5, 1);
			
			// (1.2) Set the input string for the first parameter set.
			parameters[0]->base = string_create("My name is Dave and I am a programmer.");
			if (parameters[0]->base == NULL) {
				fprintf(stderr, "[test_environment_create] Call to string_create for the first parameter failed.\n");
				test_parameter_array_destroy(parameters, TYPE_STRING_SPLICE, 3); 
				free(environment->results);
				free(environment);
				return NULL;
			} 

			// (1.3) Set the expected output string for the first parameter set
			parameters[0]->expected_output = string_create("My na");
			if (parameters[0]->expected_output == NULL) {
				fprintf(stderr, "[test_environment_create] Call to string_create for the first parameter's \"expected_output\" attribute failed.\n");
				test_parameter_array_destroy(parameters, TYPE_STRING_SPLICE, 3);
				free(environment->results);
				free(environment);
				return NULL;
			}

			// (2) Set the values for the second parameter set
			string_splice_test_parameters_set_range(parameters[1], 1, 7, 2);
			parameters[1]->base = string_create("My name is Pink and I'm really glad to meet you.");
			if (parameters[1]->base == NULL) {
				fprintf(stderr, "[test_environment_create] Call to string_create for the second parameter failed.\n");
				test_parameter_array_destroy(parameters, TYPE_STRING_SPLICE, 3);
				free(environment->results);
				free(environment);
				return NULL;
			} 
			parameters[1]->expected_output = string_create("ynm");
			if (parameters[1]->base == NULL) {
				fprintf(stderr, "[test_environment_create] Call to string_create for the second parameter failed.\n");
				test_parameter_array_destroy(parameters, TYPE_STRING_SPLICE, 3);
				free(environment->results);
				free(environment);
				return NULL;
			} 

			// (3) Set the attributes for the third parameter set
			parameters[2]->base = string_create("C is a procedural systems programming language created by Dennis Ritchie. It is my favorite programming language.");
			if (parameters[2]->base == NULL) {
				fprintf(stderr, "[test_environment_create] Call to string_create for the third parameter failed.\n");
				test_parameter_array_destroy(parameters, TYPE_STRING_SPLICE, 3);
				free(environment->results);
				free(environment);
				return NULL;
			}
			
			return environment;
			break;

		default:
			fprintf(stderr, "[test_environment_create] ERROR: TestEnvironmentType code %d is invalid/unimplemented.\n", type);
			free(environment);
			return NULL;
			break; 
	} 
}

void test_environment_destroy(struct TestEnvironment *environment) {
	if (environment == NULL) {
		fprintf(stderr, "[test_environment_destroy] Cannot free memory addressed by a NULL pointer.\n");
		return;
	} 

	if (environment->results != NULL) 	free (environment->results); 
	switch (environment->type) {
		case ENVIRONMENT_NONE:
			fprintf(stderr, "[test_environment_destroy] Cannot be certain of how to delete the dynamically allocated properties of a test environment which has an unknown type (ENVIRONMENT_NONE). There may be a memory leak due to this.\n");
			free(environment);
			break;

		case ENVIRONMENT_STRING_CREATE:

			free (environment);
			break;

		case ENVIRONMENT_STRING_SPLICE:
			test_parameter_array_destroy(environment->parameters, TYPE_STRING_SPLICE, 3);
			free(environment);
			break;
		
		default:
			fprintf(stderr, "[test_environment_destroy] TestEnvironmentType %d is an invalid/unimplemented case. The environment will be freed, but there is no guarantee that its dynamically allocated attributes will be freed correctly. A memory leak may be caused by this.\n");
			free(environment);
			break;
	}
	return;
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
