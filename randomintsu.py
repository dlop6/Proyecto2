import random

# Generate 10 million random numbers
numbers = [str(random.randint(1, 1000000)) for _ in range(1000000)]

# Join the numbers with commas
numbers_str = ','.join(numbers)

# Write the numbers to a text file
with open('random_numbers.txt', 'w') as file:
    file.write(numbers_str)