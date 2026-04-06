#define fmt_prefix(fmt) fmt

#include <ktest/test.h>
#include <kernel/types.h>
#include <kernel/printf.h>
#include <stdarg.h>
#include <kernel/types.h>

int ktest_suite_run(struct ktest_suite *suite)
{
	struct ktest_case *test_case;

	if (suite->init)
		suite->init(suite);

	printk(KTEST_LOG_LEVEL, "ktest: [%s] starting suite...\n", suite->name);

	suite->result = KTEST_SUCCESS;
	for (test_case = suite->tests; test_case->run != NULL; test_case++) {
		struct ktest test = { .name = test_case->name, .parent = suite };

		if (suite->test_init)
			suite->test_init(&test);

		ktest_log(&test, "starting test...\n");
		test_case->run(&test);
		
		ktest_log(&test, "test result: %s\n", test.result == KTEST_SUCCESS ? "SUCCESS" : "FAILURE");
		if (test.result == KTEST_FAILURE) {
			suite->result = KTEST_FAILURE;
		}

		if (suite->test_exit)
			suite->test_exit(&test);
	}

	printk(KTEST_LOG_LEVEL, "ktest: [%s] suite result: %s\n", suite->name,
	       suite->result == KTEST_SUCCESS ? "SUCCESS" : "FAILURE");

	if (suite->exit)
		suite->exit(suite);

	return 0;
}
