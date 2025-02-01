#!/usr/bin/python
from enum import Enum
from functools import cache
import sys


class SpringState(Enum):
    OPERATIONAL = "."
    DAMAGED = "#"
    UNKNOWN = "?"


#*******************************************************************************
def is_valid_condition(spring_state, damaged_spring_record):
    # bool1 = (damaged_spring_record[0] <= len(spring_state))
    # bool2 = (SpringState.OPERATIONAL.value not in spring_state[: damaged_spring_record[0]])
    # bool3 = (damaged_spring_record[0] == len(spring_state))
    # bool4 = False
    # if bool3: bool4 = (spring_state[damaged_spring_record[0]] != SpringState.DAMAGED.value)
    # rv    = (bool1 and bool2 and (bool3 or bool4))
    # return rv

    return (
        damaged_spring_record[0] <= len(spring_state)
        and
        SpringState.OPERATIONAL.value not in spring_state[: damaged_spring_record[0]]
        and
        (
            damaged_spring_record[0] == len(spring_state)
            or
            spring_state[damaged_spring_record[0]] != SpringState.DAMAGED.value
        )
    )

#*******************************************************************************
@cache
def get_valid_spring_record_combinations(spring_state, damaged_spring_record):
    if not damaged_spring_record:
        if SpringState.DAMAGED.value in spring_state:
            return 0
        else:
            return 1

    if not spring_state:
        if not damaged_spring_record:
            return 1
        else:
            return 0

    total_combinations = 0

    # if "." or "?"
    if spring_state[0] in [SpringState.OPERATIONAL.value, SpringState.UNKNOWN.value]:
        total_combinations += get_valid_spring_record_combinations(spring_state[1:], damaged_spring_record)

    # if "#" or "?"
    if spring_state[0] in [SpringState.DAMAGED.value, SpringState.UNKNOWN.value]:
        if is_valid_condition(spring_state, damaged_spring_record):
            total_combinations += get_valid_spring_record_combinations(spring_state[damaged_spring_record[0] + 1 :], damaged_spring_record[1:])

    return total_combinations

#*******************************************************************************
def sum_spring_record_combinations_unfold(spring_condition_records):
    total_combinations = 0

    for spring_record in spring_condition_records.splitlines():
        print(spring_record)
        spring_state, damaged_spring_record = spring_record.split()

        damaged_spring_record = tuple(map(int, damaged_spring_record.split(",")))

        spring_state = "?".join([spring_state] * 5)  # unfolding
        damaged_spring_record = damaged_spring_record * 5  # unfolding

        total_combinations += get_valid_spring_record_combinations(spring_state, damaged_spring_record)

    return total_combinations


#*******************************************************************************
def part_two():
    # with open("testdata2.txt") as f:
    file = sys.argv[1];
    with open(file) as f:
        spring_condition_records = f.read()

    sum = sum_spring_record_combinations_unfold(spring_condition_records)

    print(f"Total valid combinations: {sum}")


#*******************************************************************************
if __name__ == "__main__":
    part_two()
