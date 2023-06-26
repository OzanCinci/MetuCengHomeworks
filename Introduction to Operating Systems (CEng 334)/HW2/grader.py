import os
import numpy as np
import sys

rows = [0, 0, 0, 0]
cols = [0, 0, 0, 0]

max_time_diffs = [252, 721, 522, 4952, 121, 187, 212, 188, 38, 370, 652, 340, 645, 671, 87, 760, 1614, 11903, 1305, 2360, 2370, 6318, 3740, 15034, 2558, 26215, 49299, 44274, 143870, 52842, 18476, 43713, 24096, 50300, 14261, 3098, 343, 203, 4572, 22876, 692, 122661, 81, 169, 376, 1134, 7145, 13235, 40312, 149838]
#max_time_diffs = [0 for _ in range(0, 50)]
def read_matrices_from_file(file_path):
    with open(file_path, 'r') as file:
        matrices = []
        for i in range(4):
            # Read matrix size
            rows[i], cols[i] = map(int, file.readline().split())

            # Read matrix rows
            matrix = []
            for _ in range(rows[i]):
                row = list(map(int, file.readline().split()))
                matrix.append(row)

            matrices.append(matrix)

    return matrices, (rows[0]*cols[0] + rows[2]*cols[2] + rows[0] * cols[2])

def read_output_values(file_path):
    with open(file_path, 'r') as file:
        output_values = []
        for line in file:
            values = list(map(int, line.split()))
            output_values.append(values)

    return output_values

def main(args):

    # Define the number of test cases and correct answers counter
    num_test_cases = 50
    correct_answers = 0

    # Iterate over file names
    for i in range(0, num_test_cases):
        file_name = f'input{i+1}.txt'
        input_file_path = os.path.join('inputs', file_name)
        output_file_path = os.path.join('outputs', f'output{i+1}.txt')

        # Read matrices from input file
        matrices, value_count = read_matrices_from_file(input_file_path)

        # Perform matrix addition
        sum_1_2 = np.add(matrices[0], matrices[1])
        sum_3_4 = np.add(matrices[2], matrices[3])

        # Perform matrix multiplication
        result = np.matmul(sum_1_2, sum_3_4)

        # Read output values from output file
        try:
            output_values = read_output_values(output_file_path)
        except:
            print("Unknown output.")
            print(f"Test case {i+1}: FAILED")
            continue
        # Verify correctness of output values
        num_correct_values = 0
        thread_id_records = [dict(), dict(), dict()]  # One for each matrix

        # Calculated row count of first matrix and the column count of second matrix
        calculated_1_2 = [0 for _ in range(rows[0])]
        calculated_3_4 = [0 for _ in range(cols[2])]

        # Timestamps for when row of the first result matrix and the column of the second result matrix is ready
        timestamps_1_2 = [0 for _ in range(rows[0])]
        timestamps_3_4 = [0 for _ in range(cols[2])]

        for values in output_values:
            try:
                timestamp, thread_id, matrix_id, row, column, value = values
            except:
                break
            if matrix_id == 0:
                expected_value = sum_1_2[row-1][column-1]
                calculated_1_2[row-1] += 1
                if calculated_1_2[row-1] == cols[0]:
                    timestamps_1_2[row-1] = timestamp
            elif matrix_id == 1:
                expected_value = sum_3_4[row-1][column-1]
                calculated_3_4[column-1] += 1
                if calculated_3_4[column-1] == rows[2]:
                    timestamps_3_4[column-1] = timestamp
            elif matrix_id == 2:
                expected_value = result[row-1][column-1]

                values_ready_at = max(timestamps_1_2[row-1], timestamps_3_4[column-1])
                diff = timestamp - values_ready_at
                
                #if diff > max_time_diffs[i]:
                #    max_time_diffs[i] = diff
                if diff > 10 * max_time_diffs[i]:
                    print(f"Time difference between calculation of matrix 1 row {row}, matrix 2 col {column} and the matrix 3 ({row},{column}) is too much.")
                    print(f"Expected to be below {max_time_diffs[i]*4} ms but it is {diff} ms.")
                    break
            else:
                print(f"Wrong matrix id: {matrix_id} at line {output_values.index(values)}.")
                break

            if value == expected_value:
                if row in thread_id_records[matrix_id]:
                    if thread_id_records[matrix_id][row] != thread_id:
                        print(f"Wrong thread id: {thread_id} at line {output_values.index(values)}.")
                        break
                else:
                    # Record the thread_id for the corresponding row in the matrix
                    thread_id_records[matrix_id][row] = thread_id
            else:
                print(f"Wrong value at ({row}, {column}) at line {output_values.index(values)}. Expected: {expected_value}, Recorded: {value}")
                break
                
            num_correct_values += 1

        # Check if all values are correct
        if num_correct_values == value_count:
            correct_answers += 1
            print(f"Test case {i+1}: PASSED")
        else:
            print(f"Test case {i+1}: FAILED")

    # Calculate and print the grade
    grade = (correct_answers / num_test_cases) * 100
    print(f"Grade: {grade}")
    if len(args) > 0:
        print(f"{args[0]}: {grade}")
    #print([max(max_time_diffs[i], max_time_diffs2[i], max_time_diffs3[i]) for i in range(0, 50)])

if __name__ == "__main__":
    main(sys.argv[1:])


