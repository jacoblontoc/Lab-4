#!/usr/bin/env bash
set -euo pipefail
cd -- "$(dirname -- "$0")"
test_dir=$(mktemp -d)
trap 'rm -rf "$test_dir"' EXIT
gcc -Wall -Wextra -Werror -no-pie main.c sum_array.s -o "$test_dir/lab4"
[[ $("$test_dir/lab4" data.txt) == 'Sum: 5559' ]]

check_sum() {
    printf '%s\n' "$1" > "$test_dir/input.txt"
    [[ $("$test_dir/lab4" "$test_dir/input.txt") == "Sum: $2" ]]
}
check_invalid() {
    printf '%s\n' "$1" > "$test_dir/input.txt"
    if "$test_dir/lab4" "$test_dir/input.txt" > /dev/null 2>&1; then
        printf 'Unexpected success for invalid input: %s\n' "$1" >&2
        exit 1
    fi
}
check_sum $'0' 0
check_sum $'3\n-10\n4\n3' -3
check_sum $'1\n2147483647' 2147483647
check_sum $'1\n-2147483648' -2147483648
check_sum $'3\n2147483647\n1\n-1' 2147483647
check_sum $'2\r\n5\r\n6\r' 11
check_invalid ''
check_invalid $'-1'
check_invalid $'2\n5'
check_invalid $'1\nhello'
check_invalid $'1\n2147483648'
check_invalid $'1\n99999999999999999999999999999999'
check_invalid $'1\n5 6'
check_invalid $'1\n5\n6'
check_invalid $'2\n2147483647\n1'
check_invalid $'2\n-2147483648\n-1'
if "$test_dir/lab4" > /dev/null 2>&1; then exit 1; fi
if "$test_dir/lab4" "$test_dir/missing.txt" > /dev/null 2>&1; then exit 1; fi
printf 'All Lab 4 tests passed.\n'
