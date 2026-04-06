#!/usr/bin/env python3
import subprocess
import argparse
import re
from time import sleep
from math import ceil
import sys
from typing import Optional
import tempfile

def parse_log(log: str):
    test_results = re.findall(r"^ktest: \[(.*):(.*)\] test result: (SUCCESS|FAILURE)", log, re.M)
    results = {}
    for suite, test, result in test_results:
        if suite not in results:
            results[suite] = []

        results[suite].append((test, result))
    return results

def get_vm_log(timeout: int = 10, quiet: bool = False, bootargs: Optional[str] = None):
    subprocess.run(["meson", "setup", "--cross-file=meson-llvm-riscv.ini", "build"], check=True)
    subprocess.run(["meson", "compile", "-C", "build"], check=True)

    tmp = tempfile.NamedTemporaryFile()

    qemu_cmd = ["qemu-system-riscv64",
                "-nographic",
                "-m", "1G",
                "-machine", "virt", "-bios", "none",
                "-serial", f"file:{tmp.name}",
                "-kernel", "./build/src/ktest.elf"]

    if bootargs:
        qemu_cmd += ["-append", bootargs]

    if not quiet:
        print(f"running: {" ".join(qemu_cmd)}")

    p = subprocess.Popen(qemu_cmd)
    sleep(timeout)
    p.kill()
    p.wait()

    serial_output = None
    with open(tmp.name) as f:
        serial_output = f.read()

    return serial_output

class TestSuite:
    def __init__(self, name: str, testcases: list[str], weight: int):
        self.name = name
        self.testcases = testcases
        self.weight = weight
        self.results = {}

    def parse_results(self, log: str):
        test_results = re.findall(r"^ktest: \[(.*):(.*)\] test result: (SUCCESS|FAILURE)", log, re.M)
        for suite, test, result in test_results:
            if suite != self.name:
                continue
            if test not in self.testcases:
                print(f"[{self.name}] parse_results: unknown test case \"{test}\"; skipping")
            self.results[test] = result

        for test in self.testcases:
            if test not in self.results:
                self.results[test] = "MISSING"

def grade_assignment(timeout: int = 10, quiet: bool = False) -> bool:
    suites = [
            TestSuite("page_unit_tests",
                      ["test_is_aligned", "test_ppn_down", "test_ppn_up"],
                      5),
            TestSuite("virt_addr_unit_tests",
                      ["test_va_get_index"], 5),
            TestSuite("ppn_unit_tests",
                      ["test_phys_to_ppn", "test_ppn_to_phys", "test_pte_get_ppn", "test_pte_from_ppn"], 5),
            TestSuite("pte_unit_tests",
                      ["test_pte_valid", "test_pte_readable", "test_pte_writable", "test_pte_executable", "test_pte_leaf"], 5),
            TestSuite("alloc_tests",
                      ["alloc_tests_fail_without_init", "alloc_tests_sanity_checks"], 30),
            TestSuite("vm_map_tests",
                      ["vm_map_tests_kernel", "vm_map_tests_basic", "vm_map_tests_refuse_misaligned_va", "vm_map_tests_refuse_misaligned_pa", "vm_map_tests_refuse_malformed_va", "vm_map_tests_refuse_remap"], 50),
    ]

    for suite in suites:
        log = get_vm_log(timeout, quiet, bootargs=f"ktest_run={suite.name}")
        print(log)
        suite.parse_results(log)
        print(suite.results)

    print("----------[ test results ]----------")
    grade = 0
    for suite in suites:
        print(f"{suite.name}:")

        points = 0
        max_points = len(suite.results)
        for test, res in suite.results.items():
            print(f"\t{test}: {res}")
            if res == "SUCCESS":
                points += 1

        suite_grade = suite.weight * points / max_points
        print(f"\tTOTAL: {suite_grade:.2f}/{suite.weight} points")
        grade += suite_grade
        print("------------------------------------")
    print(f"FINAL GRADE: {ceil(grade)}/100 points")
    print("------------------------------------")

    return ceil(grade) == 100

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--timeout", help="timeout for the tests in seconds", default=10)
    parser.add_argument("--quiet", help="output only the final score", default=False)
    args = parser.parse_args()

    is_full_mark = grade_assignment(int(args.timeout), args.quiet)

    ret = 0 if is_full_mark else 1
    sys.exit(ret)

