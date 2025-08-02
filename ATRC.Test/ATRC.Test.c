#include "ATRC.Test.h"
#include <ATRC/ATRC.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void test_read_variable(PATRC_FD fd) {
    const char* varname = "test_variable2";
    char* value = ReadVariable(fd, varname);
    if (value == NULL || strcmp(value, "") == 0) {
        printf("[FAIL] ReadVariable: Failed to read variable '%s'\n", varname);
    }
    else {
        printf("[PASS] ReadVariable: Value of '%s' is '%s'\n", varname, value);
    }
    ATRC_FREE_MEMORY(value);
    value = NULL;
}

static void test_block_operations(PATRC_FD fd) {
    const char* blockname = "test_block";
    if (AddBlock(fd, blockname)) {
        printf("[PASS] AddBlock: Block '%s' added successfully\n", blockname);
    }
    else {
        printf("[FAIL] AddBlock: Failed to add block '%s'\n", blockname);
    }

    if (DoesExistBlock(fd, blockname)) {
        printf("[PASS] DoesExistBlock: Block '%s' exists\n", blockname);
    }
    else {
        printf("[FAIL] DoesExistBlock: Block '%s' does not exist\n", blockname);
    }

    if (RemoveBlock(fd, blockname)) {
        printf("[PASS] RemoveBlock: Block '%s' removed successfully\n", blockname);
    }
    else {
        printf("[FAIL] RemoveBlock: Failed to remove block '%s'\n", blockname);
    }
}

static void test_variable_operations(PATRC_FD fd) {
    const char* varname = "test_variable";
    const char* value = "test_value";
    

    if (AddVariable(fd, varname, value)) {
        printf("[PASS] AddVariable: Variable '%s' added successfully\n", varname);
    }
    else {
        printf("[FAIL] AddVariable: Failed to add variable '%s'\n", varname);
    }

    if (DoesExistVariable(fd, varname)) {
        printf("[PASS] DoesExistVariable: Variable '%s' exists\n", varname);
    }
    else {
        printf("[FAIL] DoesExistVariable: Variable '%s' does not exist\n", varname);
    }

    const char* new_value = "modified_value";
    if (ModifyVariable(fd, varname, new_value)) {
        printf("[PASS] ModifyVariable: Variable '%s' modified successfully\n", varname);
    }
    else {
        printf("[FAIL] ModifyVariable: Failed to modify variable '%s'\n", varname);
    }

    if (RemoveVariable(fd, varname)) {
        printf("[PASS] RemoveVariable: Variable '%s' removed successfully\n", varname);
        
    }
    else {
        printf("[FAIL] RemoveVariable: Failed to remove variable '%s'\n", varname);
    }
}

static void test_key_operations(PATRC_FD fd) {
    const char* block = "test_block";
    const char* key = "test_key";
    const char* value = "test_value";

    AddBlock(fd, block);

    if (AddKey(fd, block, key, value)) {
        printf("[PASS] AddKey: Key '%s' added to block '%s' successfully\n", key, block);
    }
    else {
        printf("[FAIL] AddKey: Failed to add key '%s' to block '%s'\n", key, block);
    }

    if (DoesExistKey(fd, block, key)) {
        printf("[PASS] DoesExistKey: Key '%s' exists in block '%s'\n", key, block);
    }
    else {
        printf("[FAIL] DoesExistKey: Key '%s' does not exist in block '%s'\n", key, block);
    }

    char* value2 = ReadKey(fd, block, key);
    if (value2 != NULL && strcmp(value2, "") != 0) {
        printf("[PASS] ReadKey: Key '%s' in block '%s' has value '%s'\n", key, block, value2);
    }
    else if (value2 == NULL) {
        printf("[FAIL] ReadKey: Failed to read key '%s' in block '%s'\n", key, block);
    }
	else {
		printf("[PASS] ReadKey: Key '%s' in block '%s' read successfully\n", key, block);
    }
	__ATRC_FREE_MEMORY_EX(value2);

    const char* new_value = "modified_value";
    if (ModifyKey(fd, block, key, new_value)) {
        printf("[PASS] ModifyKey: Key '%s' in block '%s' modified successfully\n", key, block);
    }
    else {
        printf("[FAIL] ModifyKey: Failed to modify key '%s' in block '%s'\n", key, block);
    }

    if (RemoveKey(fd, block, key)) {
        printf("[PASS] RemoveKey: Key '%s' removed from block '%s' successfully\n", key, block);
    }
    else {
        printf("[FAIL] RemoveKey: Failed to remove key '%s' from block '%s'\n", key, block);
    }

    RemoveBlock(fd, block);
}

static void test_insert(PATRC_FD fd) {
    
	char* line2 = ReadVariable(fd, "test");
	const char* args2[] = { "test", "test", NULL };
	char* res = InsertVar_S(line2, args2);
    if (res == NULL) {
		printf("InsertVar_S: Failed to insert variables\n");
    }
    printf("InsertVar_S: '%s'\n", res);
    __ATRC_FREE_MEMORY_EX(res);
	__ATRC_FREE_MEMORY_EX(line2);
}

static int run_tests() {
printf("Running ATRC API tests...\n");

PATRC_FD fd = Create_Empty_ATRC_FD();
if (fd == NULL) {
    printf("[FAIL] Create_Empty_ATRC_FD: Failed to create ATRC_FD\n");
    return 1;
}
printf("[PASS] Create_Empty_ATRC_FD: Successfully created ATRC_FD\n");

// Test file reading
if (Read(fd, "test.atrc", ATRC_READ_ONLY)) {
    printf("[PASS] Read: Successfully read file 'test.atrc'\n");
}
else {
    printf("[FAIL] Read: Failed to read file 'test.atrc'\n");
    Destroy_ATRC_FD(fd);
    return 1;
}



// Test Variable Operations
test_variable_operations(fd);

// Test Block Operations
test_block_operations(fd);

// Test Key Operations
test_key_operations(fd);

// Test Reading Variables
test_read_variable(fd);

// Test Insert
test_insert(fd);

// Clean up
Destroy_ATRC_FD(fd);
printf("[PASS] Destroy_ATRC_FD: Successfully destroyed ATRC_FD\n");

printf("Tests completed.\n");

return 0;
}
int main(int argc, char *argv[]) {
    /*
    if(run_tests() != 0) {
        printf("Tests failed.\n");
        return 1;
	}

	PATRC_FD fd = Create_ATRC_FD("../../test.atrc", ATRC_READ_ONLY);

    char *res = ReadVariable(fd, "PVD_OFFSET");
	printf("PVD_OFFSET: %s\n", res);
	double pvd_offset = atrc_math_exp(res);
	printf("PVD_OFFSET calculated: %f\n", pvd_offset);

    __ATRC_FREE_MEMORY_EX(res);
	Destroy_ATRC_FD(fd);
    */
    PATRC_FD fd = Create_ATRC_FD("../../../ATRC.Test/test.atrc", ATRC_READ_ONLY);
    if (fd == NULL) {
        return 1;
    }
    fd->AutoSave = true;
    AddBlock(fd, "NewBlock");
    bool exists = DoesExistBlock(fd, "NewBlock");
    if (exists) {
        printf("Block 'NewBlock' exists.\n");
    } else {
        printf("Block 'NewBlock' does not exist.\n");
    }
    WriteCommentToBottom(fd, "This is a comment at the bottom.");
    Destroy_ATRC_FD(fd);

    // return cpp_main();
    return 0;
}
